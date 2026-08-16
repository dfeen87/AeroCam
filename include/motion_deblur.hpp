#pragma once
/*
 * Copyright (c) 2026 Don Michael Feeney Jr.
 * Licensed under the MIT License.
 */

#include <array>
#include <vector>
#include "quaternion.hpp"
#include "imu_telemetry_parser.hpp"

namespace aerocam {

struct Mat3x3 {
    std::array<float, 9> m; // Row-major 3x3 matrix
};

struct DeblurKernel {
    int width;
    int height;
    float blur_radius_px;
    float angle_rad;
    std::vector<float> weights; // Size width x height normalized to sum to 1.0
};

class MotionDeblurEngine {
public:
    MotionDeblurEngine(float focal_length_px = 1000.0f, float frame_width = 1920.0f, float frame_height = 1080.0f);

    // Compute 3x3 frame-warp homography matrix from camera rotation delta
    Mat3x3 compute_warp_matrix(const Quaternion& q_source, const Quaternion& q_target) const;

    // Compute motion blur parameters (radius and direction) from angular velocity vector and exposure time
    DeblurKernel compute_deblur_kernel(const Vec3& gyro_rad_s, float exposure_time_sec, int kernel_size = 15) const;

    // Apply Ultra-HD zero-blur decay gain calculation
    float compute_ultrahd_decay_gain(float blur_radius, float sigma = 0.5f) const;

private:
    float fx_;
    float fy_;
    float cx_;
    float cy_;
};

} // namespace aerocam
