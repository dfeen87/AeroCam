#include "snap_spectacles_imu_fusion.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Running TestSpectaclesImuFusion..." << std::endl;

    snap::SpectaclesImuFusion fusion;

    // Simulate head nod (pitch rate pitch oscillation in gyro.x)
    aerocam::Vec3 gyro_nod{3.0f, 0.0f, 0.0f};
    aerocam::Vec3 accel_norm{0.0f, 0.0f, 9.81f};

    snap::SpectaclesHeadPose pose{};
    for (int i = 0; i < 30; ++i) {
        pose = fusion.update(gyro_nod, accel_norm, 0.05f, 0.0f, 0.0f);
    }

    std::cout << "Detected gesture: " << static_cast<int>(pose.detected_gesture)
              << " with confidence: " << pose.gesture_confidence << std::endl;

    assert(pose.detected_gesture == snap::HeadGesture::NOD);
    assert(pose.gesture_confidence > 0.8f);

    std::cout << "TestSpectaclesImuFusion PASSED!" << std::endl;
    return 0;
}
