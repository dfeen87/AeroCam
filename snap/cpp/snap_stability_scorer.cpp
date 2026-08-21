#include "snap_stability_scorer.hpp"
#include <cmath>
#include <numeric>

namespace snap {

StabilityScorer::StabilityScorer(float sigma) : sigma_(sigma) {}

float StabilityScorer::compute_score(const MotionVectorFrame& frame,
                                    const aerocam::Vec3& gyro_rad_s,
                                    const aerocam::Vec3& accel_m_s2) {
    // 1. Calculate optical flow motion magnitude
    float flow_mag_sq = frame.global_dx * frame.global_dx + frame.global_dy * frame.global_dy;

    // 2. Calculate rotational speed magnitude from gyro (rad/s)
    float gyro_mag_sq = gyro_rad_s.x * gyro_rad_s.x + gyro_rad_s.y * gyro_rad_s.y + gyro_rad_s.z * gyro_rad_s.z;

    // 3. Calculate linear acceleration jitter (deviation from 1G ~ 9.81 m/s^2)
    float accel_mag = std::sqrt(accel_m_s2.x * accel_m_s2.x + accel_m_s2.y * accel_m_s2.y + accel_m_s2.z * accel_m_s2.z);
    float accel_jitter = std::abs(accel_mag - 9.81f);

    // AILEE Gaussian decay scoring
    float total_error_sq = 100.0f * flow_mag_sq + 0.1f * gyro_mag_sq + 0.05f * (accel_jitter * accel_jitter);

    float score = std::exp(-total_error_sq / (2.0f * sigma_ * sigma_));

    if (score < 0.0f) score = 0.0f;
    if (score > 1.0f) score = 1.0f;

    last_score_ = score;

    recent_scores_.push_back(score);
    if (recent_scores_.size() > 10) {
        recent_scores_.erase(recent_scores_.begin());
    }

    return last_score_;
}

} // namespace snap
