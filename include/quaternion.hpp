#pragma once
/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 */

#include <array>
#include <cstdint>

namespace aerocam {

struct Vec3 {
    float x;
    float y;
    float z;
};

class Quaternion {
public:
    float w;
    float x;
    float y;
    float z;

    constexpr Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
    constexpr Quaternion(float w_, float x_, float y_, float z_)
        : w(w_), x(x_), y(y_), z(z_) {}

    static Quaternion identity();

    static Quaternion from_axis_angle(const Vec3& axis, float angle_rad);
    static Quaternion from_angular_velocity(const Vec3& omega_rad_s, float dt);

    Quaternion normalized() const;
    void normalize_in_place();

    Quaternion operator*(const Quaternion& rhs) const;
    Vec3 rotate(const Vec3& v) const;

    static Quaternion slerp(const Quaternion& q0, const Quaternion& q1, float t);
};

} // namespace aerocam
