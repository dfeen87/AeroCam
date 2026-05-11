#pragma once
/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 */

#include <array>
#include <cstdint>
#include <span>

namespace aerocam {

struct SensorData {
    // Raw accelerometer in milli-g (scaled later to g)
    std::int16_t accel_x;
    std::int16_t accel_y;
    std::int16_t accel_z;

    // Raw gyroscope in mdps (scaled later to dps)
    std::int16_t gyro_x;
    std::int16_t gyro_y;
    std::int16_t gyro_z;
};

class SpiBus {
public:
    virtual ~SpiBus() = default;

    virtual void select()   = 0;
    virtual void deselect() = 0;

    // Full-duplex transfer: tx.size() == rx.size()
    virtual void transfer(std::span<const std::uint8_t> tx,
                          std::span<std::uint8_t> rx) = 0;

    // Write-only convenience
    virtual void write(std::span<const std::uint8_t> tx) = 0;
};

class ICM20948_Driver {
public:
    explicit ICM20948_Driver(SpiBus& spi);

    bool initialize();
    bool configure_4khz_odr();

    bool read_sensor(SensorData& out_data);

private:
    SpiBus& spi_;

    bool write_register(std::uint8_t bank, std::uint8_t reg, std::uint8_t value);
    bool read_registers(std::uint8_t bank, std::uint8_t reg,
                        std::span<std::uint8_t> buffer);

    bool select_bank(std::uint8_t bank);

    static constexpr std::uint8_t REG_BANK_SEL = 0x7F;

    static constexpr std::uint8_t BANK_0 = 0x00;
    static constexpr std::uint8_t BANK_2 = 0x20;

    static constexpr std::uint8_t REG_ACCEL_XOUT_H = 0x2D;
    static constexpr std::uint8_t REG_GYRO_XOUT_H  = 0x33;

    static constexpr std::uint8_t REG_GYRO_SMPLRT_DIV = 0x00;
    static constexpr std::uint8_t REG_ACCEL_SMPLRT_DIV_1 = 0x10;
    static constexpr std::uint8_t REG_ACCEL_SMPLRT_DIV_2 = 0x11;

    static constexpr std::uint8_t REG_PWR_MGMT_1 = 0x06;

    bool configure_power();
    bool configure_accel_gyro_4khz();
};

} // namespace aerocam
