cmake_minimum_required(VERSION 3.20)
project(AeroCam LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Bare-metal / cross-compile flags can be extended here for STM32/ARM
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -fno-exceptions -fno-rtti")

add_library(aerocam STATIC
    src/icm20948_driver.cpp
    src/aerocam_stabilizer.cpp
    src/quaternion.cpp
)

# -------------------------------------------------------------------
# Output Formats
# -------------------------------------------------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
)

target_include_directories(aerocam PUBLIC include)
