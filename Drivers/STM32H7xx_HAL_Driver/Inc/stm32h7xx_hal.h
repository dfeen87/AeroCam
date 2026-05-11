/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 *
 * Minimal STM32H7xx HAL stub for AeroCam CI compilation.
 * Provides the types, macros, and static-inline function stubs consumed by
 * mcu_hal.hpp, spi_dma.hpp, and main.cpp when MCU=H7.
 *
 * This is NOT a functional HAL implementation – it exists solely so that
 * the firmware translation-unit(s) compile and link cleanly without the
 * full STM32CubeH7 SDK present in the repository.
 */

#ifndef STM32H7XX_HAL_H
#define STM32H7XX_HAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* CMSIS core intrinsics (bare-minimum stubs)                         */
/* ------------------------------------------------------------------ */
static inline void __disable_irq(void) {}
static inline void __enable_irq(void)  {}

/* ------------------------------------------------------------------ */
/* HAL status                                                         */
/* ------------------------------------------------------------------ */
typedef enum {
    HAL_OK      = 0x00U,
    HAL_ERROR   = 0x01U,
    HAL_BUSY    = 0x02U,
    HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

#define HAL_MAX_DELAY  0xFFFFFFFFU

/* ------------------------------------------------------------------ */
/* GPIO                                                               */
/* ------------------------------------------------------------------ */
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_TypeDef;

typedef enum { GPIO_PIN_RESET = 0, GPIO_PIN_SET } GPIO_PinState;

#define GPIO_PIN_0   ((uint16_t)0x0001U)
#define GPIO_PIN_1   ((uint16_t)0x0002U)
#define GPIO_PIN_2   ((uint16_t)0x0004U)
#define GPIO_PIN_3   ((uint16_t)0x0008U)
#define GPIO_PIN_4   ((uint16_t)0x0010U)
#define GPIO_PIN_5   ((uint16_t)0x0020U)
#define GPIO_PIN_6   ((uint16_t)0x0040U)
#define GPIO_PIN_7   ((uint16_t)0x0080U)
#define GPIO_PIN_8   ((uint16_t)0x0100U)
#define GPIO_PIN_9   ((uint16_t)0x0200U)
#define GPIO_PIN_10  ((uint16_t)0x0400U)
#define GPIO_PIN_11  ((uint16_t)0x0800U)
#define GPIO_PIN_12  ((uint16_t)0x1000U)
#define GPIO_PIN_13  ((uint16_t)0x2000U)
#define GPIO_PIN_14  ((uint16_t)0x4000U)
#define GPIO_PIN_15  ((uint16_t)0x8000U)

/* Peripheral base addresses (H7) */
#define GPIOA_BASE  0x58020000UL
#define GPIOB_BASE  0x58020400UL
#define GPIOC_BASE  0x58020800UL
#define GPIOD_BASE  0x58020C00UL
#define GPIOE_BASE  0x58021000UL

#define GPIOA  ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB  ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC  ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD  ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE  ((GPIO_TypeDef *)GPIOE_BASE)

static inline void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                                     GPIO_PinState PinState) {
    (void)GPIOx; (void)GPIO_Pin; (void)PinState;
}

/* ------------------------------------------------------------------ */
/* SPI                                                                */
/* ------------------------------------------------------------------ */
typedef struct __SPI_HandleTypeDef {
    void     *Instance;
    uint32_t  Init;
    uint8_t  *pTxBuffPtr;
    uint16_t  TxXferSize;
    uint16_t  TxXferCount;
    uint8_t  *pRxBuffPtr;
    uint16_t  RxXferSize;
    uint16_t  RxXferCount;
    uint32_t  ErrorCode;
    uint32_t  State;
} SPI_HandleTypeDef;

static inline HAL_StatusTypeDef HAL_SPI_TransmitReceive(
    SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData,
    uint16_t Size, uint32_t Timeout) {
    (void)hspi; (void)pTxData; (void)pRxData; (void)Size; (void)Timeout;
    return HAL_OK;
}

static inline HAL_StatusTypeDef HAL_SPI_Transmit(
    SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout) {
    (void)hspi; (void)pData; (void)Size; (void)Timeout;
    return HAL_OK;
}

static inline HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(
    SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size) {
    (void)hspi; (void)pTxData; (void)pRxData; (void)Size;
    return HAL_OK;
}

static inline HAL_StatusTypeDef HAL_SPI_Transmit_DMA(
    SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size) {
    (void)hspi; (void)pData; (void)Size;
    return HAL_OK;
}

/* ------------------------------------------------------------------ */
/* Timer                                                              */
/* ------------------------------------------------------------------ */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} TIM_TypeDef;

typedef struct __TIM_HandleTypeDef {
    TIM_TypeDef *Instance;
    uint32_t     Init;
    uint32_t     Channel;
    uint32_t     State;
} TIM_HandleTypeDef;

#define TIM1_BASE  0x40010000UL
#define TIM2_BASE  0x40000000UL
#define TIM3_BASE  0x40000400UL

#define TIM1  ((TIM_TypeDef *)TIM1_BASE)
#define TIM2  ((TIM_TypeDef *)TIM2_BASE)
#define TIM3  ((TIM_TypeDef *)TIM3_BASE)

#define TIM_CHANNEL_1  0x00000000U
#define TIM_CHANNEL_2  0x00000004U
#define TIM_CHANNEL_3  0x00000008U
#define TIM_CHANNEL_4  0x0000000CU

/* Matches the real HAL macro signature; accesses CCR registers via Instance */
#define __HAL_TIM_SET_COMPARE(__HANDLE__, __CHANNEL__, __COMPARE__)         \
    (((__CHANNEL__) == TIM_CHANNEL_1)                                       \
         ? ((__HANDLE__)->Instance->CCR1 = (__COMPARE__))                   \
         : ((__CHANNEL__) == TIM_CHANNEL_2)                                 \
               ? ((__HANDLE__)->Instance->CCR2 = (__COMPARE__))             \
               : ((__CHANNEL__) == TIM_CHANNEL_3)                           \
                     ? ((__HANDLE__)->Instance->CCR3 = (__COMPARE__))       \
                     : ((__HANDLE__)->Instance->CCR4 = (__COMPARE__)))

static inline HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim,
                                                   uint32_t Channel) {
    (void)htim; (void)Channel;
    return HAL_OK;
}

static inline HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim) {
    (void)htim;
    return HAL_OK;
}

/* ------------------------------------------------------------------ */
/* HAL init                                                           */
/* ------------------------------------------------------------------ */
static inline HAL_StatusTypeDef HAL_Init(void) { return HAL_OK; }

/* ------------------------------------------------------------------ */
/* RCC – H7 specific                                                  */
/* ------------------------------------------------------------------ */
/* Oscillator types */
#define RCC_OSCILLATORTYPE_NONE  0x00000000U
#define RCC_OSCILLATORTYPE_HSE   0x00000001U
#define RCC_OSCILLATORTYPE_HSI   0x00000002U
#define RCC_OSCILLATORTYPE_LSE   0x00000004U
#define RCC_OSCILLATORTYPE_LSI   0x00000008U

/* HSI states (H7 uses divider notation) */
#define RCC_HSI_OFF   0x00000000U
#define RCC_HSI_ON    0x00000001U   /* kept for compatibility */
#define RCC_HSI_DIV1  0x00000020U   /* H7: HSI / 1 */
#define RCC_HSI_DIV2  0x00000028U
#define RCC_HSI_DIV4  0x00000030U
#define RCC_HSI_DIV8  0x00000038U

#define RCC_HSICALIBRATION_DEFAULT  64U

/* PLL */
#define RCC_PLL_NONE         0x00000000U
#define RCC_PLL_OFF          0x00000001U
#define RCC_PLL_ON           0x00000002U
#define RCC_PLLSOURCE_HSI    0x00000000U
#define RCC_PLLSOURCE_HSE    0x00000003U

/* H7 VCI range (PLLRGE field) */
#define RCC_PLL1VCIRANGE_0  0x00000000U
#define RCC_PLL1VCIRANGE_1  0x00000002U
#define RCC_PLL1VCIRANGE_2  0x00000004U
#define RCC_PLL1VCIRANGE_3  0x00000006U

/* H7 VCO selection (PLLVCOSEL field) */
#define RCC_PLL1VCOWIDE    0x00000000U
#define RCC_PLL1VCOMEDIUM  0x00000001U

/* H7 PLLP is a direct integer (2, 4, 8 …) in some variants */
#define RCC_PLLP_DIV2  2U

typedef struct {
    uint32_t PLLState;
    uint32_t PLLSource;
    uint32_t PLLM;
    uint32_t PLLN;
    uint32_t PLLP;
    uint32_t PLLQ;
    uint32_t PLLR;       /* H7 extra output */
    uint32_t PLLRGE;     /* H7: VCI input range */
    uint32_t PLLVCOSEL;  /* H7: VCO range selection */
    uint32_t PLLFRACN;   /* H7: fractional divider */
} RCC_PLLInitTypeDef;

typedef struct {
    uint32_t           OscillatorType;
    uint32_t           HSEState;
    uint32_t           LSEState;
    uint32_t           HSIState;
    uint32_t           HSICalibrationValue;
    uint32_t           LSIState;
    RCC_PLLInitTypeDef PLL;
} RCC_OscInitTypeDef;

/* Clock types (H7 adds D1PCLK1 and D3PCLK1) */
#define RCC_CLOCKTYPE_SYSCLK   0x00000001U
#define RCC_CLOCKTYPE_HCLK     0x00000002U
#define RCC_CLOCKTYPE_D1PCLK1  0x00000004U   /* H7 D1 APB3 */
#define RCC_CLOCKTYPE_PCLK1    0x00000008U
#define RCC_CLOCKTYPE_PCLK2    0x00000010U
#define RCC_CLOCKTYPE_D3PCLK1  0x00000020U   /* H7 D3 APB4 */

#define RCC_SYSCLKSOURCE_HSI     0x00000000U
#define RCC_SYSCLKSOURCE_HSE     0x00000001U
#define RCC_SYSCLKSOURCE_PLLCLK  0x00000003U

/* AHB / SYSCLK prescalers */
#define RCC_SYSCLK_DIV1   0x00000000U
#define RCC_SYSCLK_DIV2   0x00000080U
#define RCC_SYSCLK_DIV4   0x00000090U

#define RCC_HCLK_DIV1   0x00000000U
#define RCC_HCLK_DIV2   0x00000008U
#define RCC_HCLK_DIV4   0x00000009U

/* APB prescalers (H7 uses per-domain names) */
#define RCC_APB3_DIV1   0x00000000U
#define RCC_APB3_DIV2   0x00000004U
#define RCC_APB3_DIV4   0x00000005U

#define RCC_APB1_DIV1   0x00000000U
#define RCC_APB1_DIV2   0x00000004U
#define RCC_APB1_DIV4   0x00000005U

#define RCC_APB2_DIV1   0x00000000U
#define RCC_APB2_DIV2   0x00000004U
#define RCC_APB2_DIV4   0x00000005U

#define RCC_APB4_DIV1   0x00000000U
#define RCC_APB4_DIV2   0x00000004U
#define RCC_APB4_DIV4   0x00000005U

/* H7 ClkInit struct includes extra APB3/APB4 prescaler fields */
typedef struct {
    uint32_t ClockType;
    uint32_t SYSCLKSource;
    uint32_t SYSCLKDivider;    /* H7: D1 domain core prescaler */
    uint32_t AHBCLKDivider;
    uint32_t APB3CLKDivider;   /* H7 D1 */
    uint32_t APB1CLKDivider;
    uint32_t APB2CLKDivider;
    uint32_t APB4CLKDivider;   /* H7 D3 */
} RCC_ClkInitTypeDef;

/* Flash latency */
#define FLASH_LATENCY_0  0x00000000U
#define FLASH_LATENCY_1  0x00000001U
#define FLASH_LATENCY_2  0x00000002U
#define FLASH_LATENCY_3  0x00000003U
#define FLASH_LATENCY_4  0x00000004U
#define FLASH_LATENCY_5  0x00000005U
#define FLASH_LATENCY_6  0x00000006U
#define FLASH_LATENCY_7  0x00000007U

/* Power (H7) */
#define PWR_LDO_SUPPLY   0x00000000U
#define PWR_SMPS_SUPPLY  0x00000040U

#define PWR_REGULATOR_VOLTAGE_SCALE0  0x00000000U
#define PWR_REGULATOR_VOLTAGE_SCALE1  0x00000040U
#define PWR_REGULATOR_VOLTAGE_SCALE2  0x00000020U
#define PWR_REGULATOR_VOLTAGE_SCALE3  0x00000000U

#define PWR_FLAG_VOSRDY  0x00002000U

#define __HAL_PWR_VOLTAGESCALING_CONFIG(__SCALE__)  do { (void)(__SCALE__); } while (0)
#define __HAL_PWR_GET_FLAG(__FLAG__)                (1U) /* stub: always ready */

static inline HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct) {
    (void)RCC_OscInitStruct;
    return HAL_OK;
}

static inline HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct,
                                                     uint32_t FLatency) {
    (void)RCC_ClkInitStruct; (void)FLatency;
    return HAL_OK;
}

/* H7-specific power extended functions */
static inline HAL_StatusTypeDef HAL_PWREx_ConfigSupply(uint32_t SupplySource) {
    (void)SupplySource;
    return HAL_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* STM32H7XX_HAL_H */
