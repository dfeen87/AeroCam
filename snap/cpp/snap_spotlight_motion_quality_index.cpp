#include "snap_spotlight_motion_quality_index.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>

namespace snap {

SpotlightMotionQualityIndex::SpotlightMotionQualityIndex(size_t window_size)
    : window_size_(window_size) {}

SpotlightMotionMetrics SpotlightMotionQualityIndex::process_frame(const MotionVectorFrame& frame,
                                                                 float scene_stability_score) {
    frame_history_.push_back(frame);
    stability_history_.push_back(scene_stability_score);

    if (frame_history_.size() > window_size_) {
        frame_history_.erase(frame_history_.begin());
        stability_history_.erase(stability_history_.begin());
    }

    return evaluate_clip(frame_history_, stability_history_);
}

SpotlightMotionMetrics SpotlightMotionQualityIndex::evaluate_clip(const std::vector<MotionVectorFrame>& frames,
                                                                  const std::vector<float>& stability_scores) {
    SpotlightMotionMetrics metrics{};
    metrics.temporal_coherence_score = 1.0f;
    metrics.scene_stability_score = 1.0f;
    metrics.overall_quality_index = 1.0f;
    metrics.passes_spotlight_threshold = true;

    if (frames.empty() || stability_scores.empty()) {
        return metrics;
    }

    // 1. Calculate temporal vector coherence (smoothness of global_dx, global_dy across consecutive frames)
    float mean_dx = 0.0f;
    float mean_dy = 0.0f;
    for (const auto& f : frames) {
        mean_dx += f.global_dx;
        mean_dy += f.global_dy;
    }
    mean_dx /= static_cast<float>(frames.size());
    mean_dy /= static_cast<float>(frames.size());

    float variance_sum = 0.0f;
    for (const auto& f : frames) {
        float diff_x = f.global_dx - mean_dx;
        float diff_y = f.global_dy - mean_dy;
        variance_sum += (diff_x * diff_x + diff_y * diff_y);
    }
    float variance = variance_sum / static_cast<float>(frames.size());

    // Higher variance = lower temporal motion coherence score
    float coherence = std::exp(-variance * 50.0f);
    if (coherence < 0.0f) coherence = 0.0f;
    if (coherence > 1.0f) coherence = 1.0f;
    metrics.temporal_coherence_score = coherence;

    // 2. Calculate average scene stability score
    float avg_stability = std::accumulate(stability_scores.begin(), stability_scores.end(), 0.0f) / static_cast<float>(stability_scores.size());
    if (avg_stability < 0.0f) avg_stability = 0.0f;
    if (avg_stability > 1.0f) avg_stability = 1.0f;
    metrics.scene_stability_score = avg_stability;

    // 3. Combined Spotlight Motion Quality Index (SMQI) = 0.6 * stability + 0.4 * coherence
    float smqi = 0.6f * avg_stability + 0.4f * coherence;
    metrics.overall_quality_index = smqi;

    // Recommended quality threshold for Spotlight feed eligibility: 0.70
    metrics.passes_spotlight_threshold = (smqi >= 0.70f);

    return metrics;
}

} // namespace snap
