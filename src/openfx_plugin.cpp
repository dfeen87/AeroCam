/*
 * Copyright (c) 2026 Don Michael Feeney Jr.
 * Licensed under the MIT License.
 */

#include "openfx_plugin.hpp"
#include <cmath>
#include <algorithm>

namespace aerocam {

AeroCamVideoStabilizer::AeroCamVideoStabilizer()
    : params_{}
    , samples_{}
    , raw_orientations_{}
    , smoothed_orientations_{}
    , deblur_engine_(params_.focal_length_px) {}

AeroCamVideoStabilizer::~AeroCamVideoStabilizer() = default;

void AeroCamVideoStabilizer::set_params(const AeroCamPluginParams& params) {
    params_ = params;
    deblur_engine_ = MotionDeblurEngine(params_.focal_length_px);
}

bool AeroCamVideoStabilizer::load_telemetry_csv(const std::string& csv_content) {
    samples_ = IMUTelemetryParser::parse_csv(csv_content);
    run_telemetry_fusion();
    return !samples_.empty();
}

bool AeroCamVideoStabilizer::load_telemetry_gpmf(const std::uint8_t* buffer, std::size_t length) {
    samples_ = IMUTelemetryParser::parse_gpmf_binary(buffer, length);
    run_telemetry_fusion();
    return !samples_.empty();
}

void AeroCamVideoStabilizer::run_telemetry_fusion() {
    if (samples_.empty()) return;

    raw_orientations_.clear();
    smoothed_orientations_.clear();

    raw_orientations_.reserve(samples_.size());
    smoothed_orientations_.reserve(samples_.size());

    Quaternion q = Quaternion::identity();
    raw_orientations_.push_back(q);

    for (std::size_t i = 1; i < samples_.size(); ++i) {
        float dt = static_cast<float>(samples_[i].timestamp_sec - samples_[i - 1].timestamp_sec);
        if (dt <= 0.0f) dt = 0.005f;

        Quaternion dq = Quaternion::from_angular_velocity(samples_[i].gyro_rad_s, dt);
        q = (q * dq).normalized();
        raw_orientations_.push_back(q);
    }

    // Apply Gaussian decay / low-pass smoothing for camera motion trajectory
    float sigma = 0.5f + (1.0f - params_.smoothness) * 2.0f;
    if (params_.mode == StabilizationMode::ULTRA_HD_ZERO_BLUR) {
        sigma = 0.2f; // Tighter smoothing to retain exact high-freq detail alignment
    }

    Quaternion current_smooth = raw_orientations_[0];
    smoothed_orientations_.push_back(current_smooth);

    for (std::size_t i = 1; i < raw_orientations_.size(); ++i) {
        float alpha = std::exp(-0.01f / (sigma * sigma));
        current_smooth = Quaternion::slerp(raw_orientations_[i], current_smooth, alpha);
        smoothed_orientations_.push_back(current_smooth);
    }
}

bool AeroCamVideoStabilizer::process_frame(double timestamp_sec, Mat3x3& out_warp_matrix, DeblurKernel& out_deblur_kernel) {
    if (samples_.empty()) return false;

    // Find closest telemetry sample
    std::size_t idx = 0;
    double min_diff = 1e9;
    for (std::size_t i = 0; i < samples_.size(); ++i) {
        double diff = std::abs(samples_[i].timestamp_sec - timestamp_sec);
        if (diff < min_diff) {
            min_diff = diff;
            idx = i;
        }
    }

    Quaternion q_raw = raw_orientations_[idx];
    Quaternion q_smooth = smoothed_orientations_[idx];

    out_warp_matrix = deblur_engine_.compute_warp_matrix(q_raw, q_smooth);

    Vec3 gyro = samples_[idx].gyro_rad_s;
    out_deblur_kernel = deblur_engine_.compute_deblur_kernel(gyro, params_.exposure_time_sec);

    return true;
}

// OpenFX C Hooks
extern "C" {

int aerocam_ofx_init() {
    return 0; // Success
}

int aerocam_ofx_process_frame(double time, const OFXImageBuffer* in_img, OFXImageBuffer* out_img, const AeroCamPluginParams* params) {
    (void)time;
    if (!in_img || !out_img || !params) return -1;
    // Standard OFX processing entry point stub
    return 0;
}

int aerocam_ofx_cleanup() {
    return 0;
}

}

} // namespace aerocam
