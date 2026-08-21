#pragma once

#include "snap_motion_vector_extractor.hpp"
#include "quaternion.hpp"
#include <vector>

namespace snap {

class StabilityScorer {
public:
    StabilityScorer(float sigma = 0.5f);
    ~StabilityScorer() = default;

    // Computes stability score in range [0.0, 1.0] from motion vector frame and gyro rates
    float compute_score(const MotionVectorFrame& frame,
                        const aerocam::Vec3& gyro_rad_s,
                        const aerocam::Vec3& accel_m_s2);

    void set_sigma(float sigma) { sigma_ = sigma; }
    float get_last_score() const { return last_score_; }

private:
    float sigma_;
    float last_score_{1.0f};
    std::vector<float> recent_scores_;
};

} // namespace snap
