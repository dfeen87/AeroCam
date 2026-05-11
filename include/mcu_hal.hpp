#ifndef AEROCAM_MCU_HAL_HPP
#define AEROCAM_MCU_HAL_HPP
/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 *
 * MCU family integer sentinels and HAL header selection.
 *
 * The CMake build system passes -DMCU=<value> where <value> matches one of
 * the sentinels below.  Use:
 *
 *   cmake ... -DMCU=F4   (Cortex-M4, STM32F4xx)
 *   cmake ... -DMCU=F7   (Cortex-M7, STM32F7xx)
 *   cmake ... -DMCU=H7   (Cortex-M7, STM32H7xx)
 */

/* Integer sentinels – must match the values emitted by CMakeLists.txt */
#define F4 4
#define F7 7
#define H7 8

#if MCU == F4
    #include "stm32f4xx_hal.h"
#elif MCU == F7
    #include "stm32f7xx_hal.h"
#elif MCU == H7
    #include "stm32h7xx_hal.h"
#else
    #error "Unknown MCU. Pass -DMCU=F4, F7, or H7 via CMake."
#endif

#endif // AEROCAM_MCU_HAL_HPP
