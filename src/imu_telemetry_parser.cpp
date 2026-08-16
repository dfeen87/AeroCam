/*
 * Copyright (c) 2026 Don Michael Feeney Jr.
 * Licensed under the MIT License.
 */

#include "imu_telemetry_parser.hpp"
#include <sstream>
#include <iostream>
#include <cmath>
#include <cstring>

namespace aerocam {

std::vector<IMUSample> IMUTelemetryParser::parse_csv(const std::string& csv_content) {
    std::vector<IMUSample> samples;
    std::istringstream stream(csv_content);
    std::string line;

    // Optional header line skip
    bool first_line = true;

    while (std::getline(stream, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream line_stream(line);
        std::string token;
        std::vector<double> values;

        while (std::getline(line_stream, token, ',')) {
            try {
                values.push_back(std::stod(token));
            } catch (...) {
                // If header or invalid token
                break;
            }
        }

        if (values.size() < 7) {
            first_line = false;
            continue;
        }

        IMUSample sample{};
        sample.timestamp_sec = values[0];
        sample.gyro_rad_s = Vec3{static_cast<float>(values[1]), static_cast<float>(values[2]), static_cast<float>(values[3])};
        sample.accel_m_s2 = Vec3{static_cast<float>(values[4]), static_cast<float>(values[5]), static_cast<float>(values[6])};

        if (values.size() >= 10) {
            sample.mag_uT = Vec3{static_cast<float>(values[7]), static_cast<float>(values[8]), static_cast<float>(values[9])};
        } else {
            sample.mag_uT = Vec3{0.0f, 0.0f, 0.0f};
        }

        samples.push_back(sample);
    }

    return samples;
}

std::vector<IMUSample> IMUTelemetryParser::parse_gyroflow_log(const std::string& json_or_csv_content) {
    // Gyroflow `.gyro` format is typically a CSV with header "t,gx,gy,gz,ax,ay,az" or JSON metadata
    // Check if CSV format first:
    return parse_csv(json_or_csv_content);
}

// FourCC helper
static constexpr uint32_t make_fourcc(char a, char b, char c, char d) {
    return (static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(b) << 16) |
           (static_cast<uint32_t>(c) << 8)  | static_cast<uint32_t>(d);
}

std::vector<IMUSample> IMUTelemetryParser::parse_gpmf_binary(const std::uint8_t* buffer, std::size_t length) {
    std::vector<IMUSample> samples;

    // Simple GPMF KLV (Key-Length-Value) parser for GYRO and ACCL streams
    // GoPro GPMF uses FourCC keys such as 'GYRO' and 'ACCL'
    if (length < 8) return samples;

    std::size_t pos = 0;
    double current_time = 0.0;
    constexpr double DT = 0.005; // Default 200 Hz sample rate assumption

    while (pos + 8 <= length) {
        uint32_t key = (static_cast<uint32_t>(buffer[pos]) << 24) |
                       (static_cast<uint32_t>(buffer[pos + 1]) << 16) |
                       (static_cast<uint32_t>(buffer[pos + 2]) << 8) |
                       static_cast<uint32_t>(buffer[pos + 3]);

        uint8_t type = buffer[pos + 4];
        uint8_t struct_size = buffer[pos + 5];
        uint16_t repeat = (static_cast<uint16_t>(buffer[pos + 6]) << 8) | buffer[pos + 7];

        std::size_t payload_size = static_cast<size_t>(struct_size) * repeat;
        // Pad size to 4-byte boundary
        std::size_t padded_payload_size = (payload_size + 3) & ~3ULL;

        if (pos + 8 + padded_payload_size > length) {
            break;
        }

        const uint8_t* payload = buffer + pos + 8;

        if (key == make_fourcc('G', 'Y', 'R', 'O') && struct_size == 6 && type == 's') { // int16 x 3
            for (uint16_t i = 0; i < repeat; ++i) {
                int16_t raw_x = static_cast<int16_t>((payload[i * 6] << 8) | payload[i * 6 + 1]);
                int16_t raw_y = static_cast<int16_t>((payload[i * 6 + 2] << 8) | payload[i * 6 + 3]);
                int16_t raw_z = static_cast<int16_t>((payload[i * 6 + 4] << 8) | payload[i * 6 + 5]);

                IMUSample sample{};
                sample.timestamp_sec = current_time;
                // Convert LSB to rad/s (typical scale factor ~ 1/16.4 deg/s to rad/s)
                constexpr float SCALE = (3.14159265f / 180.0f) / 16.4f;
                sample.gyro_rad_s = Vec3{raw_x * SCALE, raw_y * SCALE, raw_z * SCALE};
                sample.accel_m_s2 = Vec3{0.0f, 0.0f, 9.80665f}; // Default baseline
                samples.push_back(sample);

                current_time += DT;
            }
        }

        pos += 8 + padded_payload_size;
    }

    return samples;
}

} // namespace aerocam
