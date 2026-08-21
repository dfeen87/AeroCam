#include "snap_motion_vector_extractor.hpp"
#include <cmath>
#include <algorithm>

namespace snap {

MotionVectorExtractor::MotionVectorExtractor(int grid_cols, int grid_rows)
    : grid_cols_(grid_cols), grid_rows_(grid_rows) {}

void MotionVectorExtractor::set_grid_dimensions(int cols, int rows) {
    grid_cols_ = cols;
    grid_rows_ = rows;
}

MotionVectorFrame MotionVectorExtractor::extract(const uint8_t* prev_frame,
                                                 const uint8_t* curr_frame,
                                                 int width,
                                                 int height,
                                                 uint64_t timestamp_ns) {
    MotionVectorFrame frame_result;
    frame_result.timestamp_ns = timestamp_ns;
    frame_result.global_dx = 0.0f;
    frame_result.global_dy = 0.0f;
    frame_result.rotational_delta = 0.0f;

    if (!prev_frame || !curr_frame || width <= 0 || height <= 0) {
        return frame_result;
    }

    int cell_w = width / grid_cols_;
    int cell_h = height / grid_rows_;
    const int win_r = 4;    // 9x9 matching window
    const int search_r = 8; // +/- 8 pixel search displacement radius

    float sum_dx = 0.0f;
    float sum_dy = 0.0f;
    float total_conf = 0.0f;

    for (int r = 0; r < grid_rows_; ++r) {
        for (int c = 0; c < grid_cols_; ++c) {
            int cx = c * cell_w + cell_w / 2;
            int cy = r * cell_h + cell_h / 2;

            // Ensure window + search displacement stays safely within image bounds
            if (cx - win_r - search_r < 0 || cx + win_r + search_r >= width ||
                cy - win_r - search_r < 0 || cy + win_r + search_r >= height) {
                continue;
            }

            int best_dx = 0;
            int best_dy = 0;
            uint32_t min_sad = 0xFFFFFFFF;

            for (int dy = -search_r; dy <= search_r; dy += 2) {
                for (int dx = -search_r; dx <= search_r; dx += 2) {
                    uint32_t sad = 0;
                    for (int wy = -win_r; wy <= win_r; ++wy) {
                        for (int wx = -win_r; wx <= win_r; ++wx) {
                            int px_prev_x = std::clamp(cx + wx, 0, width - 1);
                            int px_prev_y = std::clamp(cy + wy, 0, height - 1);
                            int px_curr_x = std::clamp(cx + wx + dx, 0, width - 1);
                            int px_curr_y = std::clamp(cy + wy + dy, 0, height - 1);

                            int px_prev = prev_frame[px_prev_y * width + px_prev_x];
                            int px_curr = curr_frame[px_curr_y * width + px_curr_x];
                            sad += std::abs(px_prev - px_curr);
                        }
                    }
                    if (sad < min_sad) {
                        min_sad = sad;
                        best_dx = dx;
                        best_dy = dy;
                    }
                }
            }

            float confidence = 1.0f - (static_cast<float>(min_sad) / ( (2 * win_r + 1) * (2 * win_r + 1) * 255.0f ));
            if (confidence < 0.0f) confidence = 0.0f;

            OpticalFlowVector vec;
            vec.x = static_cast<float>(cx) / static_cast<float>(width);
            vec.y = static_cast<float>(cy) / static_cast<float>(height);
            vec.dx = static_cast<float>(best_dx) / static_cast<float>(width);
            vec.dy = static_cast<float>(best_dy) / static_cast<float>(height);
            vec.confidence = confidence;

            frame_result.grid_vectors.push_back(vec);

            sum_dx += vec.dx * confidence;
            sum_dy += vec.dy * confidence;
            total_conf += confidence;
        }
    }

    if (total_conf > 0.0001f) {
        frame_result.global_dx = sum_dx / total_conf;
        frame_result.global_dy = sum_dy / total_conf;
        frame_result.rotational_delta = std::atan2(frame_result.global_dy, frame_result.global_dx);
    }

    return frame_result;
}

} // namespace snap
