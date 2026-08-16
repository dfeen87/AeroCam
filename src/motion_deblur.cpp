/*
 * Copyright (c) 2026 Don Michael Feeney Jr.
 * Licensed under the MIT License.
 */

#include "motion_deblur.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace aerocam {

MotionDeblurEngine::MotionDeblurEngine(float focal_length_px, float frame_width, float frame_height)
    : fx_(focal_length_px)
    , fy_(focal_length_px)
    , cx_(frame_width * 0.5f)
    , cy_(frame_height * 0.5f) {}

Mat3x3 MotionDeblurEngine::compute_warp_matrix(const Quaternion& q_source, const Quaternion& q_target) const {
    // Relative rotation from source frame orientation to target (smoothed) frame orientation
    // q_rel = q_target * inv(q_source)
    Quaternion q_source_inv(q_source.w, -q_source.x, -q_source.y, -q_source.z);
    Quaternion q_rel = q_target * q_source_inv;
    q_rel.normalize_in_place();

    // Convert q_rel to 3x3 rotation matrix R
    float w = q_rel.w, x = q_rel.x, y = q_rel.y, z = q_rel.z;

    float r00 = 1.0f - 2.0f * (y * y + z * z);
    float r01 = 2.0f * (x * y - z * w);
    float r02 = 2.0f * (x * z + y * w);

    float r10 = 2.0f * (x * y + z * w);
    float r11 = 1.0f - 2.0f * (x * x + z * z);
    float r12 = 2.0f * (y * z - x * w);

    float r20 = 2.0f * (x * z - y * w);
    float r21 = 2.0f * (y * z + x * w);
    float r22 = 1.0f - 2.0f * (x * x + y * y);

    // Homography H = K * R * K^-1
    // K = [fx  0 cx]
    //     [ 0 fy cy]
    //     [ 0  0  1]
    //
    // K^-1 = [1/fx    0 -cx/fx]
    //        [   0 1/fy -cy/fy]
    //        [   0    0      1]

    Mat3x3 H{};
    // First line of H = K * R * K^-1 calculation:
    H.m[0] = r00 + (cx_ * r20) / fx_;
    H.m[1] = (fx_ * r01) / fy_ + (cx_ * r21) / fy_;
    H.m[2] = -r00 * cx_ - (fx_ * r01 * cy_) / fy_ + fx_ * r02 + cx_ * r22 - (cx_ * r20 * cx_) / fx_ - (cx_ * r21 * cy_) / fy_;

    H.m[3] = (fy_ * r10) / fx_ + (cy_ * r20) / fx_;
    H.m[4] = r11 + (cy_ * r21) / fy_;
    H.m[5] = -(fy_ * r10 * cx_) / fx_ - r11 * cy_ + fy_ * r12 + cy_ * r22 - (cy_ * r20 * cx_) / fx_ - (cy_ * r21 * cy_) / fy_;

    H.m[6] = r20 / fx_;
    H.m[7] = r21 / fy_;
    H.m[8] = -r20 * cx_ / fx_ - r21 * cy_ / fy_ + r22;

    return H;
}

DeblurKernel MotionDeblurEngine::compute_deblur_kernel(const Vec3& gyro_rad_s, float exposure_time_sec, int kernel_size) const {
    DeblurKernel kernel;
    kernel.width = kernel_size;
    kernel.height = kernel_size;

    // Angular velocity magnitude around Pitch (X) and Yaw (Y) causes spatial pixel translation on image plane
    // dx_px = gyro_y * focal_length * exposure_time
    // dy_px = gyro_x * focal_length * exposure_time
    float dx = gyro_rad_s.y * fx_ * exposure_time_sec;
    float dy = gyro_rad_s.x * fy_ * exposure_time_sec;

    kernel.blur_radius_px = std::sqrt(dx * dx + dy * dy);
    kernel.angle_rad = std::atan2(dy, dx);

    kernel.weights.assign(kernel_size * kernel_size, 0.0f);

    if (kernel.blur_radius_px < 0.1f) {
        // Delta function (center = 1.0)
        int center = kernel_size / 2;
        kernel.weights[center * kernel_size + center] = 1.0f;
        return kernel;
    }

    // Generate directional Gaussian point spread function along motion angle
    float center = (kernel_size - 1) * 0.5f;
    float cos_a = std::cos(kernel.angle_rad);
    float sin_a = std::sin(kernel.angle_rad);

    float sum = 0.0f;
    float sigma_parallel = std::max(1.0f, kernel.blur_radius_px * 0.5f);
    float sigma_perp = 0.8f;

    for (int y = 0; y < kernel_size; ++y) {
        for (int x = 0; x < kernel_size; ++x) {
            float px = x - center;
            float py = y - center;

            // Project onto parallel and perpendicular axes
            float p_parallel = px * cos_a + py * sin_a;
            float p_perp = -px * sin_a + py * cos_a;

            float w = std::exp(-0.5f * (p_parallel * p_parallel / (sigma_parallel * sigma_parallel) +
                                       p_perp * p_perp / (sigma_perp * sigma_perp)));

            kernel.weights[y * kernel_size + x] = w;
            sum += w;
        }
    }

    // Normalize kernel
    if (sum > 0.0f) {
        for (float& w : kernel.weights) {
            w /= sum;
        }
    }

    return kernel;
}

float MotionDeblurEngine::compute_ultrahd_decay_gain(float blur_radius, float sigma) const {
    // Inverse decay mapping for Ultra-HD zero-blur reconstruction mode:
    // As blur radius increases, gain scales down Gaussianly to prevent ringing artefacts while retaining edge sharpness
    return std::exp(-(blur_radius * blur_radius) / (2.0f * sigma * sigma));
}

} // namespace aerocam
