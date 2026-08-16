/*
 * Copyright (c) 2026 Don Michael Feeney Jr.
 * Licensed under the MIT License.
 */

#include <cassert>
#include <iostream>
#include <vector>
#include "imu_telemetry_parser.hpp"
#include "motion_deblur.hpp"
#include "openfx_plugin.hpp"

void test_csv_parser() {
    std::string csv_data =
        "t,gx,gy,gz,ax,ay,az\n"
        "0.0,0.1,0.2,0.3,0.0,0.0,9.81\n"
        "0.01,0.1,0.2,0.3,0.0,0.0,9.81\n";

    auto samples = aerocam::IMUTelemetryParser::parse_csv(csv_data);
    assert(samples.size() == 2);
    assert(samples[0].timestamp_sec == 0.0);
    assert(samples[0].gyro_rad_s.x == 0.1f);
    assert(samples[0].accel_m_s2.z == 9.81f);
}

void test_motion_deblur_engine() {
    aerocam::MotionDeblurEngine engine(1000.0f, 1920.0f, 1080.0f);

    aerocam::Quaternion q0 = aerocam::Quaternion::identity();
    aerocam::Quaternion q1 = aerocam::Quaternion::from_axis_angle({0.0f, 1.0f, 0.0f}, 0.01f);

    aerocam::Mat3x3 H = engine.compute_warp_matrix(q0, q1);
    // Matrix diagonal should be approximately 1.0
    assert(std::abs(H.m[0] - 1.0f) < 0.1f);
    assert(std::abs(H.m[4] - 1.0f) < 0.1f);
    assert(std::abs(H.m[8] - 1.0f) < 0.1f);

    aerocam::Vec3 gyro{0.5f, 0.2f, 0.0f};
    aerocam::DeblurKernel kernel = engine.compute_deblur_kernel(gyro, 0.002f, 15);
    assert(kernel.weights.size() == 225);
    assert(kernel.blur_radius_px > 0.0f);
}

void test_stabilizer_pipeline() {
    aerocam::AeroCamVideoStabilizer stabilizer;

    std::string csv_data =
        "t,gx,gy,gz,ax,ay,az\n"
        "0.0,0.05,0.01,0.0,0.0,0.0,9.81\n"
        "0.01,0.05,0.01,0.0,0.0,0.0,9.81\n"
        "0.02,0.05,0.01,0.0,0.0,0.0,9.81\n";

    bool loaded = stabilizer.load_telemetry_csv(csv_data);
    assert(loaded);
    assert(stabilizer.sample_count() == 3);

    aerocam::Mat3x3 H;
    aerocam::DeblurKernel kernel;
    bool processed = stabilizer.process_frame(0.01, H, kernel);
    assert(processed);
}

int main() {
    std::cout << "Running test_ekf & video plugin tests..." << std::endl;
    test_csv_parser();
    test_motion_deblur_engine();
    test_stabilizer_pipeline();
    std::cout << "test_ekf and plugin tests passed!" << std::endl;
    return 0;
}
