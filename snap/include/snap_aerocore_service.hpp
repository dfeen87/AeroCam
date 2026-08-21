#pragma once

#include "snap_motion_vector_extractor.hpp"
#include "snap_stability_scorer.hpp"
#include "snap_spectacles_imu_fusion.hpp"
#include "snap_spotlight_motion_quality_index.hpp"
#include <memory>

namespace snap {

struct AeroCoreMotionState {
    uint64_t timestamp_ns;
    MotionVectorFrame current_motion_frame;
    float stability_score;
    SpectaclesHeadPose head_pose;
    SpotlightMotionMetrics spotlight_metrics;
    float homography_matrix[9]; // 3x3 Homography row-major
};

class AeroCoreServiceCpp {
public:
    AeroCoreServiceCpp();
    ~AeroCoreServiceCpp() = default;

    void process_imu_sample(const aerocam::Vec3& gyro, const aerocam::Vec3& accel, float dt_sec);
    void process_camera_frame(const uint8_t* prev_frame, const uint8_t* curr_frame, int width, int height, uint64_t timestamp_ns);

    AeroCoreMotionState get_state() const;
    void compute_warp_homography(float matrix_out[9]);

private:
    MotionVectorExtractor motion_extractor_{8, 8};
    StabilityScorer stability_scorer_{0.5f};
    SpectaclesImuFusion spectacles_fusion_;
    SpotlightMotionQualityIndex spotlight_evaluator_{30};

    AeroCoreMotionState current_state_{};
    aerocam::Vec3 last_gyro_{0.0f, 0.0f, 0.0f};
    aerocam::Vec3 last_accel_{0.0f, 0.0f, 0.0f};
};

} // namespace snap
