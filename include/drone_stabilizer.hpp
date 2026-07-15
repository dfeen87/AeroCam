#pragma once
/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 */

#include "aerocam_stabilizer.hpp"
#include "quaternion.hpp"

namespace aerocam_drone {

class Drone_Stabilizer {
public:
    explicit Drone_Stabilizer(aerocam::ICM20948_Driver& driver);

    void set_mode(aerocam::StabilizerMode mode);

    // Set target altitude in meters for altitude hold mode.
    void set_target_altitude(float altitude);

    void update(float dt);

    // Returns a 4D vector: x=Roll, y=Pitch, z=Yaw, w=Thrust
    aerocam::Vec4 motor_command() const { return motor_cmd_; }

private:
    aerocam::ICM20948_Driver& driver_;
    aerocam::ExtendedKalmanFilter ekf_;
    aerocam::Quaternion attitude_;

    aerocam::StabilizerMode mode_;
    float sigma_film_;
    float sigma_aero_;
    float sigma_drone_;

    float target_altitude_;

    aerocam::Vec4 motor_cmd_;

    aerocam::Vec3 gaussian_decay(const aerocam::Vec3& error) const;
    float gaussian_scalar(float x, float sigma) const;

    aerocam::Vec3 sensor_to_body_velocity(const aerocam::SensorData& data) const;
};

} // namespace aerocam_drone
