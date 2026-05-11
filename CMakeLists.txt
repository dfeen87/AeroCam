# MIT License
cmake_minimum_required(VERSION 3.20)

project(AeroCam LANGUAGES C CXX)

# -------------------------------------------------------------------
# MCU Selection Validation
# -------------------------------------------------------------------
if(NOT (MCU_F4 OR MCU_F7 OR MCU_H7))
    message(FATAL_ERROR "You must define one MCU: -DMCU_F4=ON, -DMCU_F7=ON, or -DMCU_H7=ON")
endif()

# -------------------------------------------------------------------
# Source Files
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
# Firmware Target
# -------------------------------------------------------------------
add_executable(AeroCam ${AEROCAM_SOURCES} ${AEROCAM_HEADERS})

target_include_directories(AeroCam PUBLIC include)

# -------------------------------------------------------------------
# Post-Build Artifacts
# -------------------------------------------------------------------
add_custom_command(TARGET AeroCam POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary AeroCam.elf AeroCam.bin
    COMMAND ${CMAKE_SIZE} AeroCam.elf
)

# -------------------------------------------------------------------
# Optional Unit Tests
# -------------------------------------------------------------------
if(AEROCAM_TESTS)
    enable_testing()

    add_executable(AeroCamTests
        tests/test_quaternion.cpp
        tests/test_ekf.cpp
    )

    target_include_directories(AeroCamTests PUBLIC include)

    add_test(NAME QuaternionTests COMMAND AeroCamTests)
endif()
