# MIT License
cmake_minimum_required(VERSION 3.20)

project(AeroCam LANGUAGES C CXX)

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
        src/quaternion.cpp
    )

    set(AEROCAM_HEADERS
        include/icm20948_driver.hpp
        include/aerocam_stabilizer.hpp
        include/quaternion.hpp
    )

    add_executable(AeroCam ${AEROCAM_SOURCES} ${AEROCAM_HEADERS})
    target_include_directories(AeroCam PUBLIC include)

    add_custom_command(TARGET AeroCam POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary AeroCam.elf AeroCam.bin
        COMMAND ${CMAKE_SIZE} AeroCam.elf
    )
endif()

# -------------------------------------------------------------------
# Optional tests
# -------------------------------------------------------------------
if(AEROCAM_TESTS)
    enable_testing()

    set(AEROCAM_TEST_SOURCES
        tests/test_quaternion.cpp
        tests/test_ekf.cpp
    )

    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/test_quaternion.cpp" AND EXISTS "${CMAKE_SOURCE_DIR}/tests/test_ekf.cpp")
        add_executable(AeroCamTests ${AEROCAM_TEST_SOURCES})
        target_include_directories(AeroCamTests PUBLIC include)
        add_test(NAME AeroCamTests COMMAND AeroCamTests)
    else()
        message(WARNING "AEROCAM_TESTS is ON but tests/ sources are missing; skipping test target.")
    endif()
endif()
