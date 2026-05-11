/*
 * MIT License
 * Copyright (...)
 */

#include "icm20948_driver.hpp"
#include "aerocam_stabilizer.hpp"

// ------------------------------------------------------------
// Example SPI implementation (replace with STM32 HAL or LL)
// ------------------------------------------------------------
class SpiBus_STM32 : public aerocam::SpiBus {
public:
    void select() override {
        // Pull CS low
    }

    void deselect() override {
        // Pull CS high
    }

    void transfer(std::span<const uint8_t> tx,
                  std::span<uint8_t> rx) override {
        // Full-duplex SPI transfer
        // HAL_SPI_TransmitReceive(...) or LL_SPI
    }

    void write(std::span<const uint8_t> tx) override {
        // Write-only SPI
        // HAL_SPI_Transmit(...)
    }
};

// ------------------------------------------------------------
// Main control loop
// ------------------------------------------------------------
int main() {
    SpiBus_STM32 spi;
    aerocam::ICM20948_Driver imu(spi);
    aerocam::AeroCam_Stabilizer stabilizer(imu);

    imu.initialize();
    stabilizer.set_mode(aerocam::StabilizerMode::FILM_MODE);

    constexpr float dt = 0.001f; // 1 kHz control loop

    while (true) {
        stabilizer.update(dt);

        aerocam::Vec3 cmd = stabilizer.motor_command();

        // Send cmd.x/y/z to your motor drivers or PWM outputs
        // pwm_set(ROLL,  cmd.x);
        // pwm_set(PITCH, cmd.y);
        // pwm_set(YAW,   cmd.z);

        // Wait for next tick (timer interrupt or busy-wait)
    }
}
