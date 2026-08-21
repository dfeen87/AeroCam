#include "snap_spotlight_motion_quality_index.hpp"
#include <iostream>
#include <vector>
#include <cassert>

int main() {
    std::cout << "Running TestSpotlightMotionQualityIndex..." << std::endl;

    snap::SpotlightMotionQualityIndex evaluator(10);

    // Create synthetic smooth motion vector frames and high stability scores
    std::vector<snap::MotionVectorFrame> frames;
    std::vector<float> stability_scores;

    for (int i = 0; i < 10; ++i) {
        snap::MotionVectorFrame f;
        f.timestamp_ns = i * 33333333; // ~30 fps
        f.global_dx = 0.01f; // Steady smooth pan
        f.global_dy = 0.00f;
        f.rotational_delta = 0.0f;
        frames.push_back(f);
        stability_scores.push_back(0.95f);
    }

    snap::SpotlightMotionMetrics metrics = evaluator.evaluate_clip(frames, stability_scores);

    std::cout << "Temporal Coherence Score : " << metrics.temporal_coherence_score << std::endl;
    std::cout << "Scene Stability Score    : " << metrics.scene_stability_score << std::endl;
    std::cout << "Spotlight Quality Index  : " << metrics.overall_quality_index << std::endl;
    std::cout << "Passes Spotlight Feed?   : " << (metrics.passes_spotlight_threshold ? "YES" : "NO") << std::endl;

    assert(metrics.temporal_coherence_score > 0.8f);
    assert(metrics.scene_stability_score > 0.9f);
    assert(metrics.overall_quality_index > 0.8f);
    assert(metrics.passes_spotlight_threshold == true);

    std::cout << "TestSpotlightMotionQualityIndex PASSED!" << std::endl;
    return 0;
}
