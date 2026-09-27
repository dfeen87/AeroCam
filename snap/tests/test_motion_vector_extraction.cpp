#include "snap_motion_vector_extractor.hpp"
#include <iostream>
#include <vector>
#include <cassert>

int main() {
    std::cout << "Running TestMotionVectorExtraction..." << std::endl;

    const int width = 64;
    const int height = 64;
    std::vector<uint8_t> frame1(width * height, 128);
    std::vector<uint8_t> frame2(width * height, 128);

    // Create a moving block pattern in frame2 (shifted right by 2 pixels)
    for (int y = 20; y < 40; ++y) {
        for (int x = 20; x < 40; ++x) {
            frame1[y * width + x] = 255;
            frame2[y * width + (x + 2)] = 255;
        }
    }

    snap::MotionVectorExtractor extractor(4, 4);
    snap::MotionVectorFrame result = extractor.extract(frame1.data(), frame2.data(), width, height, 1000000);

    std::cout << "Extracted global_dx: " << result.global_dx << ", global_dy: " << result.global_dy << std::endl;
    std::cout << "Grid vectors count: " << result.grid_vectors.size() << std::endl;

    assert(!result.grid_vectors.empty());
    assert(result.timestamp_ns == 1000000);

    // Invalid grid dimensions are clamped, avoiding division by zero.
    snap::MotionVectorExtractor invalid_grid(0, -1);
    result = invalid_grid.extract(frame1.data(), frame2.data(), width, height, 2000000);
    assert(result.timestamp_ns == 2000000);

    invalid_grid.set_grid_dimensions(-2, 0);
    result = invalid_grid.extract(frame1.data(), frame2.data(), width, height, 3000000);
    assert(result.timestamp_ns == 3000000);

    std::cout << "TestMotionVectorExtraction PASSED!" << std::endl;
    return 0;
}
