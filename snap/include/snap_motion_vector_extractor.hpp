#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>
#include "quaternion.hpp"

namespace snap {

struct OpticalFlowVector {
    float x;      // Source x (normalized [0,1])
    float y;      // Source y (normalized [0,1])
    float dx;     // Displacement dx
    float dy;     // Displacement dy
    float confidence; // Vector reliability [0, 1]
};

struct MotionVectorFrame {
    uint64_t timestamp_ns;
    float global_dx;
    float global_dy;
    float rotational_delta;
    std::vector<OpticalFlowVector> grid_vectors;
};

class MotionVectorExtractor {
public:
    MotionVectorExtractor(int grid_cols = 8, int grid_rows = 8);
    ~MotionVectorExtractor() = default;

    // Processes consecutive luminance frames (grayscale uint8_t buffers)
    MotionVectorFrame extract(const uint8_t* prev_frame,
                              const uint8_t* curr_frame,
                              int width,
                              int height,
                              uint64_t timestamp_ns);

    void set_grid_dimensions(int cols, int rows);

private:
    int grid_cols_;
    int grid_rows_;
};

} // namespace snap
