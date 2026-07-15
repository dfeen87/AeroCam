/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 */

#include "drone_stabilizer.hpp"
#include <cmath>
#include <algorithm>

namespace aerocam_drone {

Drone_Stabilizer::Drone_Stabilizer(aerocam::ICM20948_Driver& driver)
    : driver_(driver)
    , ekf_()
    , attitude_(aerocam::Quaternion::identity())
    , mode_(aerocam::StabilizerMode::FILM_MODE)
    , sigma_film_(0.8f)
    , sigma_aero_(0.2f)
    , sigma_drone_(0.5f)
    , target_altitude_(0.0f)
    , motor_cmd_{0.0f, 0.0f, 0.0f, 0.0f} {}

void Drone_Stabilizer::set_mode(aerocam::StabilizerMode mode) {
    mode_ = mode;
}

void Drone_Stabilizer::set_target_altitude(float altitude) {
    target_altitude_ = altitude;
}

aerocam::Vec3 Drone_Stabilizer::sensor_to_body_velocity(const aerocam::SensorData& data) const {
    constexpr float ACCEL_LSB_TO_G = 1.0f / 16384.0f;
    constexpr float G_TO_M_S2 = 9.80665f;

    float ax = static_cast<float>(data.accel_x) * ACCEL_LSB_TO_G * G_TO_M_S2;
    float ay = static_cast<float>(data.accel_y) * ACCEL_LSB_TO_G * G_TO_M_S2;
    float az = static_cast<float>(data.accel_z) * ACCEL_LSB_TO_G * G_TO_M_S2;

    return aerocam::Vec3{ax, ay, az};
}

float Drone_Stabilizer::gaussian_scalar(float x, float sigma) const {
    float inv_two_sigma2 = 1.0f / (2.0f * sigma * sigma);
    return std::exp(-x * x * inv_two_sigma2);
}

aerocam::Vec3 Drone_Stabilizer::gaussian_decay(const aerocam::Vec3& error) const {
    float sigma = sigma_drone_;
    if (mode_ == aerocam::StabilizerMode::FILM_MODE) {
        sigma = sigma_film_;
    } else if (mode_ == aerocam::StabilizerMode::AERO_MODE) {
        sigma = sigma_aero_;
    }

    float mag = std::sqrt(error.x * error.x + error.y * error.y + error.z * error.z);
    if (mag == 0.0f) {
        return aerocam::Vec3{0.0f, 0.0f, 0.0f};
    }

    float gain = gaussian_scalar(mag, sigma);

    return aerocam::Vec3{
        -gain * error.x,
        -gain * error.y,
        -gain * error.z
    };
}

void Drone_Stabilizer::update(float dt) {
    aerocam::SensorData raw{};
    if (!driver_.read_sensor(raw)) {
        return;
    }

    aerocam::Vec3 accel_body = sensor_to_body_velocity(raw);

    ekf_.predict(accel_body, dt);

    aerocam::Vec3 v_meas = accel_body;
    ekf_.update(v_meas);

    aerocam::Vec3 v_est = ekf_.velocity();

    aerocam::Vec3 desired_v{0.0f, 0.0f, 0.0f};
    aerocam::Vec3 error{
        v_est.x - desired_v.x,
        v_est.y - desired_v.y,
        v_est.z - desired_v.z
    };

    aerocam::Vec3 torque_cmd = gaussian_decay(error);

    float thrust_cmd = 0.0f;

    if (mode_ == aerocam::StabilizerMode::DRONE_MODE) {
        // Implement a basic Altitude Hold using velocity Z for damping.
        // We assume we want to maintain the current altitude if velocity is 0
        // Or simple thrust control structure:
        float z_velocity_error = 0.0f - v_est.z;

        // Very basic altitude hold: P controller on velocity
        float kp_thrust = 1.5f;

        thrust_cmd = kp_thrust * z_velocity_error;

        // Add hover throttle base (assuming 0.5 is normalized hover thrust)
        thrust_cmd += 0.5f;

        // Clamp thrust
        thrust_cmd = std::max(0.0f, std::min(1.0f, thrust_cmd));
    }

    motor_cmd_ = aerocam::Vec4{torque_cmd.x, torque_cmd.y, torque_cmd.z, thrust_cmd};
}

} // namespace aerocam_drone
