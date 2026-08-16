#pragma once
/*
 * Copyright (c) 2026 Don Michael Feeney Jr.
 * Licensed under the MIT License.
 */

#include <cstdint>
#include <string>
#include <vector>
#include "motion_deblur.hpp"
#include "imu_telemetry_parser.hpp"

namespace aerocam {

enum class StabilizationMode : std::uint8_t {
    OFF = 0,
    SMOOTH_FILM = 1,
    AGGRESSIVE_AERO = 2,
    ULTRA_HD_ZERO_BLUR = 3
};

struct AeroCamPluginParams {
    StabilizationMode mode = StabilizationMode::SMOOTH_FILM;
    float smoothness = 0.5f;          // 0.0 to 1.0
    float exposure_time_sec = 0.002f;  // 1/500s exposure
    float focal_length_px = 1200.0f;  // Lens focal length in pixels
    bool enable_deblur = true;
    bool crop_to_fit = true;
};

// C++ Standalone Engine & OpenFX Bridge Interface
class AeroCamVideoStabilizer {
public:
    AeroCamVideoStabilizer();
    ~AeroCamVideoStabilizer();

    // Load IMU telemetry log (CSV, Gyroflow .gyro, or GoPro GPMF)
    bool load_telemetry_csv(const std::string& csv_content);
    bool load_telemetry_gpmf(const std::uint8_t* buffer, std::size_t length);

    // Set engine parameters
    void set_params(const AeroCamPluginParams& params);

    // Compute per-frame transformation and deblur kernel for frame index k at timestamp_sec
    bool process_frame(double timestamp_sec, Mat3x3& out_warp_matrix, DeblurKernel& out_deblur_kernel);

    // Filter telemetry sequence with EKF and Gaussian decay
    void run_telemetry_fusion();

    std::size_t sample_count() const { return samples_.size(); }

private:
    AeroCamPluginParams params_;
    std::vector<IMUSample> samples_;
    std::vector<Quaternion> raw_orientations_;
    std::vector<Quaternion> smoothed_orientations_;
    MotionDeblurEngine deblur_engine_;
};

// C-ABI Compatible OpenFX Plugin Hooks
extern "C" {
    struct OFXImageBuffer {
        int width;
        int height;
        int row_bytes;
        float* rgba_data;
    };

    int aerocam_ofx_init();
    int aerocam_ofx_process_frame(double time, const OFXImageBuffer* in_img, OFXImageBuffer* out_img, const AeroCamPluginParams* params);
    int aerocam_ofx_cleanup();
}

} // namespace aerocam
