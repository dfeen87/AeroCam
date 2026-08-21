#include "snap_spectacles_imu_fusion.hpp"
#include <cmath>
#include <algorithm>

namespace snap {

SpectaclesImuFusion::SpectaclesImuFusion() {
    reset();
}

void SpectaclesImuFusion::reset() {
    current_orientation_ = aerocam::Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    filtered_gyro_ = aerocam::Vec3{0.0f, 0.0f, 0.0f};
    filtered_accel_ = aerocam::Vec3{0.0f, 0.0f, 9.81f};
    nod_accumulator_ = 0.0f;
    shake_accumulator_ = 0.0f;
    tilt_accumulator_ = 0.0f;
    lean_accumulator_ = 0.0f;
    last_gesture_ = HeadGesture::NONE;
    gesture_timer_ = 0.0f;
}

SpectaclesHeadPose SpectaclesImuFusion::update(const aerocam::Vec3& gyro,
                                               const aerocam::Vec3& accel,
                                               float dt_sec,
                                               float optical_flow_dx,
                                               float optical_flow_dy) {
    SpectaclesHeadPose pose;
    pose.timestamp_ns = 0; // Filled by caller/system clock

    // Exponential smoothing (low-pass filter) on sensor inputs
    const float alpha = 0.8f;
    filtered_gyro_.x = alpha * filtered_gyro_.x + (1.0f - alpha) * gyro.x;
    filtered_gyro_.y = alpha * filtered_gyro_.y + (1.0f - alpha) * gyro.y;
    filtered_gyro_.z = alpha * filtered_gyro_.z + (1.0f - alpha) * gyro.z;

    filtered_accel_.x = alpha * filtered_accel_.x + (1.0f - alpha) * accel.x;
    filtered_accel_.y = alpha * filtered_accel_.y + (1.0f - alpha) * accel.y;
    filtered_accel_.z = alpha * filtered_accel_.z + (1.0f - alpha) * accel.z;

    // Integrate angular velocity into orientation quaternion
    aerocam::Quaternion dq = aerocam::Quaternion::from_angular_velocity(filtered_gyro_, dt_sec);
    current_orientation_ = (current_orientation_ * dq).normalized();

    // Fuse optical flow delta for pitch/yaw micro-corrections
    float flow_pitch_corr = optical_flow_dy * 0.1f;
    float flow_yaw_corr = optical_flow_dx * 0.1f;

    aerocam::Quaternion q_pitch = aerocam::Quaternion::from_axis_angle(aerocam::Vec3{1.0f, 0.0f, 0.0f}, flow_pitch_corr);
    aerocam::Quaternion q_yaw = aerocam::Quaternion::from_axis_angle(aerocam::Vec3{0.0f, 1.0f, 0.0f}, flow_yaw_corr);
    aerocam::Quaternion flow_corr = q_pitch * q_yaw;

    current_orientation_ = (current_orientation_ * flow_corr).normalized();

    pose.orientation = current_orientation_;
    pose.angular_velocity = filtered_gyro_;
    pose.linear_acceleration = filtered_accel_;

    // Calculate AR display alignment stabilization (inverse/conjugate of unit quaternion)
    pose.ar_alignment_correction = aerocam::Quaternion(
        current_orientation_.w,
        -current_orientation_.x,
        -current_orientation_.y,
        -current_orientation_.z
    );

    // Detect gestures
    float gesture_confidence = 0.0f;
    pose.detected_gesture = detect_gesture(dt_sec, gesture_confidence);
    pose.gesture_confidence = gesture_confidence;

    return pose;
}

HeadGesture SpectaclesImuFusion::detect_gesture(float dt_sec, float& confidence) {
    if (gesture_timer_ > 0.0f) {
        gesture_timer_ -= dt_sec;
        confidence = 0.9f;
        return last_gesture_;
    }

    // Nod: Pitch oscillation (gyro.x)
    if (std::abs(filtered_gyro_.x) > 1.5f) {
        nod_accumulator_ += std::abs(filtered_gyro_.x) * dt_sec;
    } else {
        nod_accumulator_ *= 0.9f;
    }

    // Shake: Yaw oscillation (gyro.y)
    if (std::abs(filtered_gyro_.y) > 1.8f) {
        shake_accumulator_ += std::abs(filtered_gyro_.y) * dt_sec;
    } else {
        shake_accumulator_ *= 0.9f;
    }

    // Tilt: Roll tilt (gyro.z)
    if (std::abs(filtered_gyro_.z) > 1.5f) {
        tilt_accumulator_ += std::abs(filtered_gyro_.z) * dt_sec;
    } else {
        tilt_accumulator_ *= 0.9f;
    }

    // Lean: Dynamic linear accel deviation from static 1G gravity
    float dynamic_accel_z = std::abs(filtered_accel_.z - 9.81f);
    if (dynamic_accel_z > 4.0f) {
        lean_accumulator_ += dynamic_accel_z * dt_sec;
    } else {
        lean_accumulator_ *= 0.9f;
    }

    const float threshold = 0.4f;

    if (nod_accumulator_ > threshold) {
        nod_accumulator_ = 0.0f;
        gesture_timer_ = 0.5f;
        confidence = 0.95f;
        last_gesture_ = HeadGesture::NOD;
        return HeadGesture::NOD;
    }

    if (shake_accumulator_ > threshold) {
        shake_accumulator_ = 0.0f;
        gesture_timer_ = 0.5f;
        confidence = 0.95f;
        last_gesture_ = HeadGesture::SHAKE;
        return HeadGesture::SHAKE;
    }

    if (tilt_accumulator_ > threshold) {
        HeadGesture g = (filtered_gyro_.z > 0.0f) ? HeadGesture::TILT_RIGHT : HeadGesture::TILT_LEFT;
        tilt_accumulator_ = 0.0f;
        gesture_timer_ = 0.5f;
        confidence = 0.90f;
        last_gesture_ = g;
        return g;
    }

    if (lean_accumulator_ > threshold) {
        HeadGesture g = (filtered_accel_.z > 9.81f) ? HeadGesture::LEAN_FORWARD : HeadGesture::LEAN_BACKWARD;
        lean_accumulator_ = 0.0f;
        gesture_timer_ = 0.5f;
        confidence = 0.85f;
        last_gesture_ = g;
        return g;
    }

    confidence = 0.0f;
    last_gesture_ = HeadGesture::NONE;
    return HeadGesture::NONE;
}

} // namespace snap
