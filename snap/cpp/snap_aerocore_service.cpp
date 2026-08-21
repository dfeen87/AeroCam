#include "snap_aerocore_service.hpp"
#include <cstring>
#include <cmath>

namespace snap {

AeroCoreServiceCpp::AeroCoreServiceCpp() {}

void AeroCoreServiceCpp::process_imu_sample(const aerocam::Vec3& gyro, const aerocam::Vec3& accel, float dt_sec) {
    last_gyro_ = gyro;
    last_accel_ = accel;

    // Update Spectacles head pose fusion
    current_state_.head_pose = spectacles_fusion_.update(
        gyro, accel, dt_sec,
        current_state_.current_motion_frame.global_dx,
        current_state_.current_motion_frame.global_dy
    );
}

void AeroCoreServiceCpp::process_camera_frame(const uint8_t* prev_frame,
                                             const uint8_t* curr_frame,
                                             int width,
                                             int height,
                                             uint64_t timestamp_ns) {
    current_state_.timestamp_ns = timestamp_ns;

    // Extract optical flow motion vectors
    current_state_.current_motion_frame = motion_extractor_.extract(prev_frame, curr_frame, width, height, timestamp_ns);

    // Compute stability score
    current_state_.stability_score = stability_scorer_.compute_score(
        current_state_.current_motion_frame,
        last_gyro_,
        last_accel_
    );

    // Compute Spotlight motion metrics
    current_state_.spotlight_metrics = spotlight_evaluator_.process_frame(
        current_state_.current_motion_frame,
        current_state_.stability_score
    );

    // Compute warp homography
    compute_warp_homography(current_state_.homography_matrix);
}

AeroCoreMotionState AeroCoreServiceCpp::get_state() const {
    return current_state_;
}

void AeroCoreServiceCpp::compute_warp_homography(float matrix_out[9]) {
    // Construct real-time 3x3 homography matrix for micro-correction warp
    float dx = -current_state_.current_motion_frame.global_dx;
    float dy = -current_state_.current_motion_frame.global_dy;
    float rot = -current_state_.current_motion_frame.rotational_delta * 0.1f;

    float cos_r = std::cos(rot);
    float sin_r = std::sin(rot);

    // Row-major 3x3 affine transformation matrix
    matrix_out[0] = cos_r;  matrix_out[1] = -sin_r; matrix_out[2] = dx;
    matrix_out[3] = sin_r;  matrix_out[4] = cos_r;  matrix_out[5] = dy;
    matrix_out[6] = 0.0f;   matrix_out[7] = 0.0f;   matrix_out[8] = 1.0f;
}

} // namespace snap
