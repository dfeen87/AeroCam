# MIT License
cmake_minimum_required(VERSION 3.20)

project(AeroCam LANGUAGES C CXX)

# -------------------------------------------------------------------
# MCU validation (after toolchain is loaded)
# -------------------------------------------------------------------
if(NOT DEFINED MCU)
    message(FATAL_ERROR "Define -DMCU=F4, F7, or H7 when configuring.")
endif()

message(STATUS "Building AeroCam for MCU=${MCU}")

# -------------------------------------------------------------------
# Sources
# -------------------------------------------------------------------
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

# -------------------------------------------------------------------
# Firmware target
# -------------------------------------------------------------------
add_executable(AeroCam ${AEROCAM_SOURCES} ${AEROCAM_HEADERS})

target_include_directories(AeroCam PUBLIC include)

# -------------------------------------------------------------------
# Post-build artifacts
# -------------------------------------------------------------------
add_custom_command(TARGET AeroCam POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary AeroCam.elf AeroCam.bin
    COMMAND ${CMAKE_SIZE} AeroCam.elf
)

# -------------------------------------------------------------------
# Optional tests
# -------------------------------------------------------------------
if(AEROCAM_TESTS)
    enable_testing()

    add_executable(AeroCamTests
        tests/test_quaternion.cpp
        tests/test_ekf.cpp
    )

    target_include_directories(AeroCamTests PUBLIC include)

    add_test(NAME AeroCamTests COMMAND AeroCamTests)
endif()
