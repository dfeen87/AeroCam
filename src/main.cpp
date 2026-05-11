/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 */

#include <cstdint>
#include <array>
#include <span>

#include "icm20948_driver.hpp"
#include "aerocam_stabilizer.hpp"

// ============================================================
// MCU SELECTION
// ============================================================
// Define one of these in your CMake or compiler flags:
//
//   -DMCU_F4
//   -DMCU_F7
//   -DMCU_H7
//
// ============================================================

#if defined(MCU_F4)
    #include "stm32f4xx_hal.h"
#elif defined(MCU_F7)
    #include "stm32f7xx_hal.h"
#elif defined(MCU_H7)
    #include "stm32h7xx_hal.h"
#else
    #error "No MCU family defined. Define MCU_F4, MCU_F7, or MCU_H7."
#endif

// ============================================================
// External HAL handles (CubeMX normally generates these)
// ============================================================
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;   // PWM
extern TIM_HandleTypeDef htim2;   // 1 kHz control loop

// ============================================================
// Motor Axis Enum
// ============================================================
enum class MotorAxis : uint8_t {
    ROLL,
    PITCH,
    YAW
};

// ============================================================
// SPI Bus Implementation
// ============================================================
class SpiBus_STM32 : public aerocam::SpiBus {
public:
    SpiBus_STM32(SPI_HandleTypeDef* h, GPIO_TypeDef* port, uint16_t pin)
        : hspi_(h), cs_port_(port), cs_pin_(pin) {}

    void select() override {
        HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_RESET);
    }

    void deselect() override {
        HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_SET);
    }

    void transfer(std::span<const uint8_t> tx,
                  std::span<uint8_t> rx) override {
        HAL_SPI_TransmitReceive(hspi_,
                                const_cast<uint8_t*>(tx.data()),
                                rx.data(),
                                tx.size(),
                                HAL_MAX_DELAY);
    }

    void write(std::span<const uint8_t> tx) override {
        HAL_SPI_Transmit(hspi_,
                         const_cast<uint8_t*>(tx.data()),
                         tx.size(),
                         HAL_MAX_DELAY);
    }

private:
    SPI_HandleTypeDef* hspi_;
    GPIO_TypeDef* cs_port_;
    uint16_t cs_pin_;
};

// ============================================================
// Motor Driver (PWM)
// ============================================================
class MotorDriver {
public:
    MotorDriver(TIM_HandleTypeDef* h,
                uint32_t roll_ch,
                uint32_t pitch_ch,
                uint32_t yaw_ch)
        : htim_(h), roll_(roll_ch), pitch_(pitch_ch), yaw_(yaw_ch) {}

    void init() {
        HAL_TIM_PWM_Start(htim_, roll_);
        HAL_TIM_PWM_Start(htim_, pitch_);
        HAL_TIM_PWM_Start(htim_, yaw_);
    }

    void set(MotorAxis axis, float v) {
        constexpr float MIN = 1000.0f;
        constexpr float MAX = 2000.0f;

        if (v > 1.0f) v = 1.0f;
        if (v < -1.0f) v = -1.0f;

        float mid = (MIN + MAX) * 0.5f;
        float amp = (MAX - MIN) * 0.5f;
        uint32_t pulse = static_cast<uint32_t>(mid + amp * v);

        switch (axis) {
            case MotorAxis::ROLL:  __HAL_TIM_SET_COMPARE(htim_, roll_,  pulse); break;
            case MotorAxis::PITCH: __HAL_TIM_SET_COMPARE(htim_, pitch_, pulse); break;
            case MotorAxis::YAW:   __HAL_TIM_SET_COMPARE(htim_, yaw_,   pulse); break;
        }
    }

private:
    TIM_HandleTypeDef* htim_;
    uint32_t roll_, pitch_, yaw_;
};

// ============================================================
// Control Loop Tick Flag
// ============================================================
static volatile bool g_tick = false;

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    if (htim->Instance == TIM2) {
        g_tick = true;
    }
}

// ============================================================
// Static System Objects (no dynamic allocation)
// ============================================================
static SpiBus_STM32* g_spi = nullptr;
static aerocam::ICM20948_Driver* g_imu = nullptr;
static aerocam::AeroCam_Stabilizer* g_stab = nullptr;
static MotorDriver* g_motors = nullptr;

// ============================================================
// MCU-SPECIFIC INITIALIZATION BLOCKS
// ============================================================

#if defined(MCU_F4)
static void SystemClock_Config() {
    // F4 PLL config
}
static void MX_SPI1_Init() {
    // SPI1 init for F4
}
static void MX_TIM1_Init() {
    // PWM timer init
}
static void MX_TIM2_Init() {
    // 1 kHz control loop timer
}

#elif defined(MCU_F7)
static void SystemClock_Config() {
    // F7 PLL config
}
static void MX_SPI1_Init() {
    // SPI1 init for F7
}
static void MX_TIM1_Init() {
    // PWM timer init
}
static void MX_TIM2_Init() {
    // 1 kHz control loop timer
}

#elif defined(MCU_H7)
static void SystemClock_Config() {
    // H7 PLL config
}
static void MX_SPI1_Init() {
    // SPI1 init for H7
}
static void MX_TIM1_Init() {
    // PWM timer init
}
static void MX_TIM2_Init() {
    // 1 kHz control loop timer
}
#endif

// ============================================================
// MAIN
// ============================================================
int main() {
    HAL_Init();
    SystemClock_Config();

    MX_SPI1_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();

    // IMU CS pin (adapt to your board)
    constexpr uint16_t IMU_CS_PIN = GPIO_PIN_4;
    GPIO_TypeDef* IMU_CS_PORT = GPIOA;

    static SpiBus_STM32 spi(&hspi1, IMU_CS_PORT, IMU_CS_PIN);
    static aerocam::ICM20948_Driver imu(spi);
    static aerocam::AeroCam_Stabilizer stab(imu);
    static MotorDriver motors(&htim1,
                              TIM_CHANNEL_1,
                              TIM_CHANNEL_2,
                              TIM_CHANNEL_3);

    g_spi = &spi;
    g_imu = &imu;
    g_stab = &stab;
    g_motors = &motors;

    motors.init();

    if (!imu.initialize()) {
        while (true) {
            // error LED blink
        }
    }

    stab.set_mode(aerocam::StabilizerMode::FILM_MODE);

    HAL_TIM_Base_Start_IT(&htim2);

    constexpr float dt = 0.001f;

    while (true) {
        if (g_tick) {
            g_tick = false;

            stab.update(dt);
            aerocam::Vec3 cmd = stab.motor_command();

            motors.set(MotorAxis::ROLL,  cmd.x);
            motors.set(MotorAxis::PITCH, cmd.y);
            motors.set(MotorAxis::YAW,   cmd.z);
        }

        // Optional low-power sleep
        // __WFI();
    }
}

