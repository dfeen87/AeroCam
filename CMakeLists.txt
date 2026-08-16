# Copyright (c) 2026 Don Michael Feeney Jr.
# Licensed under the MIT License.
cmake_minimum_required(VERSION 3.20)

project(AeroCam VERSION 3.0.0 LANGUAGES C CXX ASM)

option(AEROCAM_TESTS "Enable native unit tests" OFF)

# -------------------------------------------------------------------
# Firmware build
# -------------------------------------------------------------------
if(NOT AEROCAM_TESTS)
    if(NOT DEFINED MCU)
        message(FATAL_ERROR "Define -DMCU=F4, F7, or H7 when configuring.")
    endif()

    message(STATUS "Building AeroCam for MCU=${MCU}")

    set(AEROCAM_SOURCES
        src/main.cpp
        src/icm20948_driver.cpp
        src/aerocam_stabilizer.cpp
        src/drone_stabilizer.cpp
        src/quaternion.cpp
        src/imu_telemetry_parser.cpp
        src/motion_deblur.cpp
        src/openfx_plugin.cpp
        src/startup_aerocam.s
    )

    set(AEROCAM_HEADERS
        include/icm20948_driver.hpp
        include/aerocam_stabilizer.hpp
        include/drone_stabilizer.hpp
        include/quaternion.hpp
        include/imu_telemetry_parser.hpp
        include/motion_deblur.hpp
        include/openfx_plugin.hpp
    )

    add_executable(AeroCam ${AEROCAM_SOURCES} ${AEROCAM_HEADERS})
    target_include_directories(AeroCam PUBLIC include)

    if(MCU STREQUAL "F4")
        target_compile_definitions(AeroCam PRIVATE MCU=4)
        target_include_directories(AeroCam PRIVATE
            ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc)
    elseif(MCU STREQUAL "F7")
        target_compile_definitions(AeroCam PRIVATE MCU=7)
        target_include_directories(AeroCam PRIVATE
            ${CMAKE_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Inc)
    elseif(MCU STREQUAL "H7")
        target_compile_definitions(AeroCam PRIVATE MCU=8)
        target_include_directories(AeroCam PRIVATE
            ${CMAKE_SOURCE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc)
    else()
        message(FATAL_ERROR "Unknown MCU '${MCU}'. Use F4, F7, or H7.")
    endif()

    add_custom_command(TARGET AeroCam POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:AeroCam> AeroCam.bin
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:AeroCam>
    )
endif()

# -------------------------------------------------------------------
# Optional tests
# -------------------------------------------------------------------
if(AEROCAM_TESTS)
    enable_testing()

    set(AEROCAM_LIB_SOURCES
        src/aerocam_stabilizer.cpp
        src/drone_stabilizer.cpp
        src/quaternion.cpp
        src/imu_telemetry_parser.cpp
        src/motion_deblur.cpp
        src/openfx_plugin.cpp
    )

    add_library(AeroCamLib STATIC ${AEROCAM_LIB_SOURCES})
    target_compile_features(AeroCamLib PUBLIC cxx_std_20)
    target_include_directories(AeroCamLib PUBLIC include)

    add_executable(TestQuaternion tests/test_quaternion.cpp)
    target_link_libraries(TestQuaternion PRIVATE AeroCamLib)
    add_test(NAME TestQuaternion COMMAND TestQuaternion)

    add_executable(TestEKF tests/test_ekf.cpp)
    target_link_libraries(TestEKF PRIVATE AeroCamLib)
    add_test(NAME TestEKF COMMAND TestEKF)
endif()
