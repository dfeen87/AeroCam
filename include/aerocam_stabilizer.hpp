#pragma once
/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 */

#include <array>
#include <cstdint>
#include <span>

#include "icm20948_driver.hpp"
#include "quaternion.hpp"

namespace aerocam {

enum class StabilizerMode : std::uint8_t {
    FILM_MODE,
    AERO_MODE
};

struct StateVector {
    Vec3 position;
    Vec3 velocity;
    Quaternion orientation;
};

class ExtendedKalmanFilter {
public:
    ExtendedKalmanFilter();

    void reset();

    void predict(const Vec3& accel_body, float dt);
    void update(const Vec3& measured_velocity);

    Vec3 velocity() const { return state_.velocity; }

private:
    StateVector state_;

    std::array<float, 9> P_;
    std::array<float, 9> Q_;
    std::array<float, 9> R_;

    void rk4_integrate(const Vec3& accel_world, float dt);
};

class AeroCam_Stabilizer {
public:
    explicit AeroCam_Stabilizer(ICM20948_Driver& driver);

    void set_mode(StabilizerMode mode);

    void update(float dt);

    Vec3 motor_command() const { return motor_cmd_; }

private:
    ICM20948_Driver& driver_;
    ExtendedKalmanFilter ekf_;
    Quaternion attitude_;

    StabilizerMode mode_;
    float sigma_film_;
    float sigma_aero_;

    Vec3 motor_cmd_;

    Vec3 gaussian_decay(const Vec3& error) const;
    float gaussian_scalar(float x, float sigma) const;

    Vec3 sensor_to_body_velocity(const SensorData& data) const;
};

} // namespace aerocam
