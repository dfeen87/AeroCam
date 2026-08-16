/*
 * Copyright (c) 2026 Don Michael Feeney Jr.
 * Licensed under the MIT License.
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include "quaternion.hpp"

void test_quaternion_identity() {
    aerocam::Quaternion q = aerocam::Quaternion::identity();
    assert(q.w == 1.0f);
    assert(q.x == 0.0f);
    assert(q.y == 0.0f);
    assert(q.z == 0.0f);
}

void test_quaternion_rotation() {
    aerocam::Vec3 axis{0.0f, 0.0f, 1.0f}; // Rotate around Z axis
    float angle = 3.14159265f / 2.0f;     // 90 degrees
    aerocam::Quaternion q = aerocam::Quaternion::from_axis_angle(axis, angle);

    aerocam::Vec3 v{1.0f, 0.0f, 0.0f};
    aerocam::Vec3 v_rot = q.rotate(v);

    // 90 deg rotation of (1, 0, 0) around Z should yield (0, 1, 0)
    assert(std::abs(v_rot.x - 0.0f) < 1e-4f);
    assert(std::abs(v_rot.y - 1.0f) < 1e-4f);
    assert(std::abs(v_rot.z - 0.0f) < 1e-4f);
}

int main() {
    std::cout << "Running test_quaternion..." << std::endl;
    test_quaternion_identity();
    test_quaternion_rotation();
    std::cout << "test_quaternion passed!" << std::endl;
    return 0;
}
