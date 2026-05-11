/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 */

#include "aerocam_stabilizer.hpp"

#include <cmath>

namespace aerocam {

// -------- ExtendedKalmanFilter --------

ExtendedKalmanFilter::ExtendedKalmanFilter()
    : state_{}
    , P_{}
    , Q_{}
    , R_{} {
    reset();
}

void ExtendedKalmanFilter::reset() {
    state_.position = Vec3{0.0f, 0.0f, 0.0f};
    state_.velocity = Vec3{0.0f, 0.0f, 0.0f};
    state_.orientation = Quaternion::identity();

    P_.fill(0.0f);
    Q_.fill(0.0f);
    R_.fill(0.0f);

    P_[0] = P_[4] = P_[8] = 1.0f;
    Q_[0] = Q_[4] = Q_[8] = 0.01f;
    R_[0] = R_[4] = R_[8] = 0.1f;
}

void ExtendedKalmanFilter::rk4_integrate(const Vec3& accel_world, float dt) {
    auto deriv = [&](const StateVector& s) -> StateVector {
        StateVector ds{};
        ds.position = s.velocity;
        ds.velocity = accel_world;
        ds.orientation = Quaternion::from_angular_velocity(Vec3{0.0f, 0.0f, 0.0f}, 0.0f);
        return ds;
    };

    StateVector k1 = deriv(state_);
    StateVector s2{
        Vec3{state_.position.x + 0.5f * dt * k1.position.x,
             state_.position.y + 0.5f * dt * k1.position.y,
             state_.position.z + 0.5f * dt * k1.position.z},
        Vec3{state_.velocity.x + 0.5f * dt * k1.velocity.x,
             state_.velocity.y + 0.5f * dt * k1.velocity.y,
             state_.velocity.z + 0.5f * dt * k1.velocity.z},
        state_.orientation
    };

    StateVector k2 = deriv(s2);
    StateVector s3{
        Vec3{state_.position.x + 0.5f * dt * k2.position.x,
             state_.position.y + 0.5f * dt * k2.position.y,
             state_.position.z + 0.5f * dt * k2.position.z},
        Vec3{state_.velocity.x + 0.5f * dt * k2.velocity.x,
             state_.velocity.y + 0.5f * dt * k2.velocity.y,
             state_.velocity.z + 0.5f * dt * k2.velocity.z},
        state_.orientation
    };

    StateVector k3 = deriv(s3);
    StateVector s4{
        Vec3{state_.position.x + dt * k3.position.x,
             state_.position.y + dt * k3.position.y,
             state_.position.z + dt * k3.position.z},
        Vec3{state_.velocity.x + dt * k3.velocity.x,
             state_.velocity.y + dt * k3.velocity.y,
             state_.velocity.z + dt * k3.velocity.z},
        state_.orientation
    };

    StateVector k4 = deriv(s4);

    state_.position.x += (dt / 6.0f) * (k1.position.x + 2.0f * k2.position.x + 2.0f * k3.position.x + k4.position.x);
    state_.position.y += (dt / 6.0f) * (k1.position.y + 2.0f * k2.position.y + 2.0f * k3.position.y + k4.position.y);
    state_.position.z += (dt / 6.0f) * (k1.position.z + 2.0f * k2.position.z + 2.0f * k3.position.z + k4.position.z);

    state_.velocity.x += (dt / 6.0f) * (k1.velocity.x + 2.0f * k2.velocity.x + 2.0f * k3.velocity.x + k4.velocity.x);
    state_.velocity.y += (dt / 6.0f) * (k1.velocity.y + 2.0f * k2.velocity.y + 2.0f * k3.velocity.y + k4.velocity.y);
    state_.velocity.z += (dt / 6.0f) * (k1.velocity.z + 2.0f * k2.velocity.z + 2.0f * k3.velocity.z + k4.velocity.z);
}

void ExtendedKalmanFilter::predict(const Vec3& accel_body, float dt) {
    Vec3 accel_world = accel_body;
    rk4_integrate(accel_world, dt);
}

void ExtendedKalmanFilter::update(const Vec3& measured_velocity) {
    Vec3 innovation{
        measured_velocity.x - state_.velocity.x,
        measured_velocity.y - state_.velocity.y,
        measured_velocity.z - state_.velocity.z
    };

    float k = 0.5f;

    state_.velocity.x += k * innovation.x;
    state_.velocity.y += k * innovation.y;
    state_.velocity.z += k * innovation.z;
}

// -------- AeroCam_Stabilizer --------

AeroCam_Stabilizer::AeroCam_Stabilizer(ICM20948_Driver& driver)
    : driver_(driver)
    , ekf_()
    , attitude_(Quaternion::identity())
    , mode_(StabilizerMode::FILM_MODE)
    , sigma_film_(0.8f)
    , sigma_aero_(0.2f)
    , motor_cmd_{0.0f, 0.0f, 0.0f} {}

void AeroCam_Stabilizer::set_mode(StabilizerMode mode) {
    mode_ = mode;
}

Vec3 AeroCam_Stabilizer::sensor_to_body_velocity(const SensorData& data) const {
    constexpr float ACCEL_LSB_TO_G = 1.0f / 16384.0f;
    constexpr float G_TO_M_S2 = 9.80665f;

    float ax = static_cast<float>(data.accel_x) * ACCEL_LSB_TO_G * G_TO_M_S2;
    float ay = static_cast<float>(data.accel_y) * ACCEL_LSB_TO_G * G_TO_M_S2;
    float az = static_cast<float>(data.accel_z) * ACCEL_LSB_TO_G * G_TO_M_S2;

    return Vec3{ax, ay, az};
}

float AeroCam_Stabilizer::gaussian_scalar(float x, float sigma) const {
    float inv_two_sigma2 = 1.0f / (2.0f * sigma * sigma);
    return std::exp(-x * x * inv_two_sigma2);
}

Vec3 AeroCam_Stabilizer::gaussian_decay(const Vec3& error) const {
    float sigma = (mode_ == StabilizerMode::FILM_MODE) ? sigma_film_ : sigma_aero_;

    float mag = std::sqrt(error.x * error.x + error.y * error.y + error.z * error.z);
    if (mag == 0.0f) {
        return Vec3{0.0f, 0.0f, 0.0f};
    }

    float gain = gaussian_scalar(mag, sigma);

    return Vec3{
        -gain * error.x,
        -gain * error.y,
        -gain * error.z
    };
}

void AeroCam_Stabilizer::update(float dt) {
    SensorData raw{};
    if (!driver_.read_sensor(raw)) {
        return;
    }

    Vec3 accel_body = sensor_to_body_velocity(raw);

    ekf_.predict(accel_body, dt);

    Vec3 v_meas = accel_body;
    ekf_.update(v_meas);

    Vec3 v_est = ekf_.velocity();

    Vec3 desired_v{0.0f, 0.0f, 0.0f};
    Vec3 error{
        v_est.x - desired_v.x,
        v_est.y - desired_v.y,
        v_est.z - desired_v.z
    };

    motor_cmd_ = gaussian_decay(error);
}

} // namespace aerocam
