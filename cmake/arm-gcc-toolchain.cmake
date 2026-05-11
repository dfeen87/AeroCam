# MIT License
# AeroCam ARM GCC Toolchain File

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# -------------------------------------------------------------------
# Expect: -DMCU=F4 / F7 / H7
# -------------------------------------------------------------------
if(NOT DEFINED MCU)
    message(FATAL_ERROR "Define -DMCU=F4, F7, or H7 when configuring.")
endif()

set(ARM_GCC_PREFIX arm-none-eabi-)

set(CMAKE_C_COMPILER   ${ARM_GCC_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${ARM_GCC_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${ARM_GCC_PREFIX}gcc)
set(CMAKE_AR           ${ARM_GCC_PREFIX}ar)
set(CMAKE_OBJCOPY      ${ARM_GCC_PREFIX}objcopy)
set(CMAKE_OBJDUMP      ${ARM_GCC_PREFIX}objdump)
set(CMAKE_SIZE         ${ARM_GCC_PREFIX}size)

# -------------------------------------------------------------------
# CPU flags based on MCU
# -------------------------------------------------------------------
if(MCU STREQUAL "F4")
    set(CPU_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
elseif(MCU STREQUAL "F7")
    set(CPU_FLAGS "-mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard")
elseif(MCU STREQUAL "H7")
    set(CPU_FLAGS "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard")
else()
    message(FATAL_ERROR "Unknown MCU: ${MCU}. Use F4, F7, or H7.")
endif()

set(COMMON_FLAGS "
    ${CPU_FLAGS}
    -mthumb
    -O3
    -ffast-math
    -fno-exceptions
    -fno-rtti
    -fdata-sections
    -ffunction-sections
    -Wall -Wextra
")

set(CMAKE_C_FLAGS   "${COMMON_FLAGS} -std=c11")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++20")

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/AeroCam.ld)

set(CMAKE_EXE_LINKER_FLAGS "
    ${CPU_FLAGS}
    -T${LINKER_SCRIPT}
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
")
