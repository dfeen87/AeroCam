/*
 * MIT License
 * Copyright (...)
 */

#include <cstdint>
#include <array>
#include <span>

#include "icm20948_driver.hpp"
#include "aerocam_stabilizer.hpp"

// ------------------------------------------------------------
// Hardware-specific includes (adapt to your STM32 family)
// ------------------------------------------------------------
// #include "stm32h7xx_hal.h"   // Example
// #include "main.h"

// ------------------------------------------------------------
// Global hardware handles (to be provided by CubeMX or manually)
// ------------------------------------------------------------
// extern SPI_HandleTypeDef hspi1;
// extern TIM_HandleTypeDef htim1;   // For PWM
// extern TIM_HandleTypeDef htim2;   // For control loop tick (1 kHz)

// ------------------------------------------------------------
// Motor channels
// ------------------------------------------------------------
enum class MotorAxis : std::uint8_t {
    ROLL  = 0,
    PITCH = 1,
    YAW   = 2
};

// ------------------------------------------------------------
// SPI bus implementation for STM32 HAL
// ------------------------------------------------------------
class SpiBus_STM32 : public aerocam::SpiBus {
public:
    explicit SpiBus_STM32(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin)
        : hspi_(hspi), cs_port_(cs_port), cs_pin_(cs_pin) {}

    void select() override {
        // CS low
        HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_RESET);
    }

    void deselect() override {
        // CS high
        HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_SET);
    }

    void transfer(std::span<const std::uint8_t> tx,
                  std::span<std::uint8_t> rx) override {
        // Blocking full-duplex transfer
        HAL_SPI_TransmitReceive(hspi_,
                                const_cast<std::uint8_t*>(tx.data()),
                                rx.data(),
                                static_cast<uint16_t>(tx.size()),
                                HAL_MAX_DELAY);
    }

    void write(std::span<const std::uint8_t> tx) override {
        HAL_SPI_Transmit(hspi_,
                         const_cast<std::uint8_t*>(tx.data()),
                         static_cast<uint16_t>(tx.size()),
                         HAL_MAX_DELAY);
    }

private:
    SPI_HandleTypeDef* hspi_;
    GPIO_TypeDef* cs_port_;
    uint16_t cs_pin_;
};

// ------------------------------------------------------------
// Motor driver (PWM) abstraction
// ------------------------------------------------------------
class MotorDriver {
public:
    MotorDriver(TIM_HandleTypeDef* htim,
                std::uint32_t roll_channel,
                std::uint32_t pitch_channel,
                std::uint32_t yaw_channel)
        : htim_(htim)
        , roll_ch_(roll_channel)
        , pitch_ch_(pitch_channel)
        , yaw_ch_(yaw_channel) {}

    void init() {
        HAL_TIM_PWM_Start(htim_, roll_ch_);
        HAL_TIM_PWM_Start(htim_, pitch_ch_);
        HAL_TIM_PWM_Start(htim_, yaw_ch_);
    }

    void set_command(MotorAxis axis, float value) {
        // value in [-1, 1] mapped to PWM range
        constexpr float MIN_PULSE = 1000.0f;
        constexpr float MAX_PULSE = 2000.0f;

        float v = value;
        if (v > 1.0f)  v = 1.0f;
        if (v < -1.0f) v = -1.0f;

        float mid = 0.5f * (MIN_PULSE + MAX_PULSE);
        float amp = 0.5f * (MAX_PULSE - MIN_PULSE);
        float pulse = mid + amp * v;

        std::uint32_t ccr = static_cast<std::uint32_t>(pulse);

        switch (axis) {
        case MotorAxis::ROLL:
            __HAL_TIM_SET_COMPARE(htim_, roll_ch_, ccr);
            break;
        case MotorAxis::PITCH:
            __HAL_TIM_SET_COMPARE(htim_, pitch_ch_, ccr);
            break;
        case MotorAxis::YAW:
            __HAL_TIM_SET_COMPARE(htim_, yaw_ch_, ccr);
            break;
        }
    }

private:
    TIM_HandleTypeDef* htim_;
    std::uint32_t roll_ch_;
    std::uint32_t pitch_ch_;
    std::uint32_t yaw_ch_;
};

// ------------------------------------------------------------
// Control loop timing
// ------------------------------------------------------------
static volatile bool g_control_tick = false;

// Called from timer ISR at 1 kHz
extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    // if (htim->Instance == TIM2) { // Example: TIM2 is control timer
    //     g_control_tick = true;
    // }
}

// ------------------------------------------------------------
// Static system objects (no dynamic allocation)
// ------------------------------------------------------------

// These will be bound in main() once HAL handles are valid
static SpiBus_STM32*      g_spi_bus      = nullptr;
static aerocam::ICM20948_Driver* g_imu   = nullptr;
static aerocam::AeroCam_Stabilizer* g_stab = nullptr;
static MotorDriver*       g_motors       = nullptr;

// ------------------------------------------------------------
// System initialization stubs
// (Replace with CubeMX-generated or custom init code)
// ------------------------------------------------------------
static void SystemClock_Config();
static void MX_GPIO_Init();
static void MX_SPI1_Init();
static void MX_TIM1_Init();
static void MX_TIM2_Init();

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main() {
    // HAL init
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();

    // External handles (from MX_*_Init)
    extern SPI_HandleTypeDef hspi1;
    extern TIM_HandleTypeDef htim1;
    extern TIM_HandleTypeDef htim2;

    // Chip select pin for IMU (adapt to your board)
    constexpr uint16_t IMU_CS_PIN = GPIO_PIN_4;
    GPIO_TypeDef* IMU_CS_PORT = GPIOA;

    static SpiBus_STM32 spi_bus(&hspi1, IMU_CS_PORT, IMU_CS_PIN);
    static aerocam::ICM20948_Driver imu(spi_bus);
    static aerocam::AeroCam_Stabilizer stabilizer(imu);
    static MotorDriver motors(&htim1,
                              TIM_CHANNEL_1, // ROLL
                              TIM_CHANNEL_2, // PITCH
                              TIM_CHANNEL_3  // YAW
    );

    g_spi_bus = &spi_bus;
    g_imu     = &imu;
    g_stab    = &stabilizer;
    g_motors  = &motors;

    motors.init();

    if (!imu.initialize()) {
        // Hard fault, blink LED or stay here
        while (true) {
            // error_indicator();
        }
    }

    stabilizer.set_mode(aerocam::StabilizerMode::FILM_MODE);

    // Start control timer at 1 kHz
    HAL_TIM_Base_Start_IT(&htim2);

    constexpr float CONTROL_DT = 0.001f; // 1 kHz

    while (true) {
        if (g_control_tick) {
            g_control_tick = false;

            stabilizer.update(CONTROL_DT);

            aerocam::Vec3 cmd = stabilizer.motor_command();

            // Map velocity-based command to motor outputs
            // Here we just clamp and feed directly
            g_motors->set_command(MotorAxis::ROLL,  cmd.x);
            g_motors->set_command(MotorAxis::PITCH, cmd.y);
            g_motors->set_command(MotorAxis::YAW,   cmd.z);
        }

        // Optional: low-power sleep until next interrupt
        // __WFI();
    }
}

// ------------------------------------------------------------
// Minimal init stubs (replace with real ones)
// ------------------------------------------------------------
static void SystemClock_Config() {
    // Configure PLL, buses, etc.
}

static void MX_GPIO_Init() {
    // Configure CS pin, LEDs, etc.
}

static void MX_SPI1_Init() {
    // hspi1.Instance = SPI1;
    // hspi1.Init.Mode = SPI_MODE_MASTER;
    // hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    // hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    // hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    // hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
    // hspi1.Init.NSS = SPI_NSS_SOFT;
    // hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    // hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    // HAL_SPI_Init(&hspi1);
}

static void MX_TIM1_Init() {
    // Configure PWM timer for motors
}

static void MX_TIM2_Init() {
    // Configure base timer for 1 kHz interrupt
    // Example:
    // htim2.Instance = TIM2;
    // htim2.Init.Prescaler = ...;
    // htim2.Init.Period    = ...;
    // HAL_TIM_Base_Init(&htim2);
}
