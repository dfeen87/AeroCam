#pragma once
/*
 * Copyright (c) 2026 Don Michael Feeney Jr.
 * Licensed under the MIT License.
 */

#include <cstdint>
#include <string>
#include <vector>
#include "quaternion.hpp"

namespace aerocam {

enum class TelemetrySourceType : std::uint8_t {
    GO_PRO_GPMF,
    GYROFLOW_LOG,
    GENERIC_CSV
};

struct IMUSample {
    double timestamp_sec; // timestamp in seconds
    Vec3 gyro_rad_s;      // angular velocity in radians/sec (x, y, z)
    Vec3 accel_m_s2;      // acceleration in m/s^2 (x, y, z)
    Vec3 mag_uT;          // magnetometer in microteslas (optional)
};

class IMUTelemetryParser {
public:
    static std::vector<IMUSample> parse_csv(const std::string& csv_content);
    static std::vector<IMUSample> parse_gyroflow_log(const std::string& json_or_csv_content);
    static std::vector<IMUSample> parse_gpmf_binary(const std::uint8_t* buffer, std::size_t length);
};

} // namespace aerocam
