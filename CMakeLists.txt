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

    # Translate the CMake MCU string into the integer sentinel consumed by
    # mcu_hal.hpp (and any other firmware header that uses #if MCU == F4).
    # Also wire in the matching STM32 HAL Inc directory so the family-specific
    # HAL header (stm32f4xx_hal.h / stm32f7xx_hal.h / stm32h7xx_hal.h) is found.
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

    set(MISSING_TEST_SOURCES "")
    foreach(TEST_SOURCE IN LISTS AEROCAM_TEST_SOURCES)
        if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${TEST_SOURCE}")
            list(APPEND MISSING_TEST_SOURCES "${TEST_SOURCE}")
        endif()
    endforeach()

    if(NOT MISSING_TEST_SOURCES)
        add_executable(AeroCamTests ${AEROCAM_TEST_SOURCES})
        target_include_directories(AeroCamTests PUBLIC include)
        add_test(NAME AeroCamTests COMMAND AeroCamTests)
    else()
        message(WARNING "AEROCAM_TESTS is ON but test sources are missing: ${MISSING_TEST_SOURCES}; skipping test target.")
    endif()
endif()
