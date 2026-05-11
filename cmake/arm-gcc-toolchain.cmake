# MIT License
# AeroCam ARM GCC Toolchain File

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(ARM_GCC_PREFIX arm-none-eabi-)

find_program(ARM_GCC     NAMES ${ARM_GCC_PREFIX}gcc REQUIRED)
find_program(ARM_GXX     NAMES ${ARM_GCC_PREFIX}g++ REQUIRED)
find_program(ARM_AR      NAMES ${ARM_GCC_PREFIX}ar REQUIRED)
find_program(ARM_OBJCOPY NAMES ${ARM_GCC_PREFIX}objcopy REQUIRED)
find_program(ARM_OBJDUMP NAMES ${ARM_GCC_PREFIX}objdump REQUIRED)
find_program(ARM_SIZE    NAMES ${ARM_GCC_PREFIX}size REQUIRED)

set(CMAKE_C_COMPILER   ${ARM_GCC}     CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER ${ARM_GXX}     CACHE FILEPATH "C++ compiler" FORCE)
set(CMAKE_ASM_COMPILER ${ARM_GCC}     CACHE FILEPATH "ASM compiler" FORCE)
set(CMAKE_AR           ${ARM_AR}      CACHE FILEPATH "Archiver" FORCE)
set(CMAKE_OBJCOPY      ${ARM_OBJCOPY} CACHE FILEPATH "Objcopy" FORCE)
set(CMAKE_OBJDUMP      ${ARM_OBJDUMP} CACHE FILEPATH "Objdump" FORCE)
set(CMAKE_SIZE         ${ARM_SIZE}    CACHE FILEPATH "Size tool" FORCE)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES MCU)

# -------------------------------------------------------------------
# Expect: -DMCU=F4 / F7 / H7
# -------------------------------------------------------------------
if(NOT DEFINED MCU)
    message(FATAL_ERROR "Define -DMCU=F4, F7, or H7 when configuring.")
endif()

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

# -------------------------------------------------------------------
# Flags (single-line to avoid Ninja parse errors)
# -------------------------------------------------------------------
set(COMMON_FLAGS "${CPU_FLAGS} -mthumb -O3 -ffast-math -fno-exceptions -fno-rtti -fdata-sections -ffunction-sections -Wall -Wextra")

set(CMAKE_C_FLAGS   "${COMMON_FLAGS} -std=c11")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++20")

# -------------------------------------------------------------------
# Linker
# -------------------------------------------------------------------
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/AeroCam.ld)

set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} -T${LINKER_SCRIPT} -Wl,--gc-sections -Wl,-Map=${PROJECT_NAME}.map")
