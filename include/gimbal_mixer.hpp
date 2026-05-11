#pragma once
/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 */

#include <cstddef>

#include "quaternion.hpp"

namespace aerocam {

class GimbalMixer {
public:
    struct MotorCommands {
        float roll;
        float pitch;
        float yaw;

        constexpr float operator[](std::size_t index) const noexcept {
            return index == 0U ? roll : (index == 1U ? pitch : yaw);
        }
    };

    constexpr GimbalMixer() = default;

    constexpr MotorCommands mix(const Vec3& command) const noexcept {
        return MotorCommands{
            saturate(command.x),
            saturate(command.y),
            saturate(command.z),
        };
    }

    constexpr MotorCommands operator()(const Vec3& command) const noexcept {
        return mix(command);
    }

    static constexpr Vec3 to_vec3(const MotorCommands& commands) noexcept {
        return Vec3{commands.roll, commands.pitch, commands.yaw};
    }

private:
    static constexpr float saturate(float value) noexcept {
        return value > 1.0f ? 1.0f : (value < -1.0f ? -1.0f : value);
    }
};

} // namespace aerocam
