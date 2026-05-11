/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 */

#include "quaternion.hpp"

#include <cmath>

namespace aerocam {

Quaternion Quaternion::identity() {
    return Quaternion{};
}

Quaternion Quaternion::from_axis_angle(const Vec3& axis, float angle_rad) {
    float half = 0.5f * angle_rad;
    float s = std::sin(half);

    float norm = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    float inv_norm = (norm > 0.0f) ? (1.0f / norm) : 0.0f;

    return Quaternion{
        std::cos(half),
        axis.x * inv_norm * s,
        axis.y * inv_norm * s,
        axis.z * inv_norm * s
    };
}

Quaternion Quaternion::from_angular_velocity(const Vec3& omega_rad_s, float dt) {
    Vec3 axis{omega_rad_s.x, omega_rad_s.y, omega_rad_s.z};
    float mag = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    if (mag == 0.0f) {
        return Quaternion::identity();
    }
    float angle = mag * dt;
    axis.x /= mag;
    axis.y /= mag;
    axis.z /= mag;
    return from_axis_angle(axis, angle);
}

Quaternion Quaternion::normalized() const {
    float n = std::sqrt(w*w + x*x + y*y + z*z);
    if (n == 0.0f) {
        return Quaternion::identity();
    }
    float inv = 1.0f / n;
    return Quaternion{w * inv, x * inv, y * inv, z * inv};
}

void Quaternion::normalize_in_place() {
    float n = std::sqrt(w*w + x*x + y*y + z*z);
    if (n == 0.0f) {
        *this = Quaternion::identity();
        return;
    }
    float inv = 1.0f / n;
    w *= inv;
    x *= inv;
    y *= inv;
    z *= inv;
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const {
    return Quaternion{
        w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
        w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
        w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
        w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w
    };
}

Vec3 Quaternion::rotate(const Vec3& v) const {
    Quaternion qv{0.0f, v.x, v.y, v.z};
    Quaternion inv{w, -x, -y, -z};
    Quaternion res = (*this) * qv * inv;
    return Vec3{res.x, res.y, res.z};
}

Quaternion Quaternion::slerp(const Quaternion& q0, const Quaternion& q1, float t) {
    float cos_theta = q0.w * q1.w + q0.x * q1.x + q0.y * q1.y + q0.z * q1.z;

    Quaternion q1_mod = q1;
    if (cos_theta < 0.0f) {
        cos_theta = -cos_theta;
        q1_mod.w = -q1_mod.w;
        q1_mod.x = -q1_mod.x;
        q1_mod.y = -q1_mod.y;
        q1_mod.z = -q1_mod.z;
    }

    const float EPS = 1e-5f;
    if (1.0f - cos_theta < EPS) {
        float w_ = q0.w + t * (q1_mod.w - q0.w);
        float x_ = q0.x + t * (q1_mod.x - q0.x);
        float y_ = q0.y + t * (q1_mod.y - q0.y);
        float z_ = q0.z + t * (q1_mod.z - q0.z);
        Quaternion q{w_, x_, y_, z_};
        return q.normalized();
    }

    float theta = std::acos(cos_theta);
    float sin_theta = std::sin(theta);
    float w0 = std::sin((1.0f - t) * theta) / sin_theta;
    float w1 = std::sin(t * theta) / sin_theta;

    Quaternion q{
        w0 * q0.w + w1 * q1_mod.w,
        w0 * q0.x + w1 * q1_mod.x,
        w0 * q0.y + w1 * q1_mod.y,
        w0 * q0.z + w1 * q1_mod.z
    };
    return q.normalized();
}

} // namespace aerocam
