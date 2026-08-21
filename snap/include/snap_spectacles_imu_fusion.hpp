#pragma once

#include "quaternion.hpp"
#include <cstdint>

namespace snap {

enum class HeadGesture {
    NONE = 0,
    NOD,
    SHAKE,
    TILT_LEFT,
    TILT_RIGHT,
    LEAN_FORWARD,
    LEAN_BACKWARD
};

struct SpectaclesHeadPose {
    uint64_t timestamp_ns;
    aerocam::Quaternion orientation; // Head orientation quaternion
    aerocam::Vec3 angular_velocity;   // rad/s
    aerocam::Vec3 linear_acceleration;// m/s^2
    HeadGesture detected_gesture;
    float gesture_confidence;         // [0, 1]
    aerocam::Quaternion ar_alignment_correction; // Quaternion for AR display stabilization
};

class SpectaclesImuFusion {
public:
    SpectaclesImuFusion();
    ~SpectaclesImuFusion() = default;

    // Updates state from 1 kHz IMU sample + optical flow motion delta
    SpectaclesHeadPose update(const aerocam::Vec3& gyro,
                              const aerocam::Vec3& accel,
                              float dt_sec,
                              float optical_flow_dx = 0.0f,
                              float optical_flow_dy = 0.0f);

    void reset();

private:
    aerocam::Quaternion current_orientation_{1.0f, 0.0f, 0.0f, 0.0f};
    aerocam::Vec3 filtered_gyro_{0.0f, 0.0f, 0.0f};
    aerocam::Vec3 filtered_accel_{0.0f, 0.0f, 0.0f};

    // Gesture detection buffers & thresholds
    float nod_accumulator_{0.0f};
    float shake_accumulator_{0.0f};
    float tilt_accumulator_{0.0f};
    float lean_accumulator_{0.0f};
    HeadGesture last_gesture_{HeadGesture::NONE};
    float gesture_timer_{0.0f};

    HeadGesture detect_gesture(float dt_sec, float& confidence);
};

} // namespace snap
