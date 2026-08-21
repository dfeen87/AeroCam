#pragma once

#include "snap_motion_vector_extractor.hpp"
#include <vector>
#include <cstdint>

namespace snap {

struct SpotlightMotionMetrics {
    float temporal_coherence_score; // Consistency of motion vectors over time [0, 1]
    float scene_stability_score;    // Aerocam EKF stability metric [0, 1]
    float overall_quality_index;    // Spotlight Motion Quality Index (SMQI) [0, 1]
    bool passes_spotlight_threshold;// Meets recommended quality for Spotlight feed promotion
};

class SpotlightMotionQualityIndex {
public:
    SpotlightMotionQualityIndex(size_t window_size = 30);
    ~SpotlightMotionQualityIndex() = default;

    // Ingests motion frame and stability score to compute continuous quality index
    SpotlightMotionMetrics process_frame(const MotionVectorFrame& frame, float scene_stability_score);

    // Compute metrics over entire clip buffer
    SpotlightMotionMetrics evaluate_clip(const std::vector<MotionVectorFrame>& frames,
                                         const std::vector<float>& stability_scores);

private:
    size_t window_size_;
    std::vector<MotionVectorFrame> frame_history_;
    std::vector<float> stability_history_;
};

} // namespace snap
