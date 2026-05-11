/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 */

#include "icm20948_driver.hpp"

namespace aerocam {

ICM20948_Driver::ICM20948_Driver(SpiBus& spi)
    : spi_(spi) {}

bool ICM20948_Driver::initialize() {
    if (!configure_power()) {
        return false;
    }
    if (!configure_4khz_odr()) {
        return false;
    }
    return true;
}

bool ICM20948_Driver::configure_4khz_odr() {
    if (!configure_accel_gyro_4khz()) {
        return false;
    }
    return true;
}

bool ICM20948_Driver::read_sensor(SensorData& out_data) {
    std::array<std::uint8_t, 12> buffer{}; // 6 accel + 6 gyro

    if (!select_bank(BANK_0)) {
        return false;
    }

    if (!read_registers(BANK_0, REG_ACCEL_XOUT_H, buffer)) {
        return false;
    }

    auto to_int16 = [](std::uint8_t hi, std::uint8_t lo) -> std::int16_t {
        return static_cast<std::int16_t>((static_cast<std::uint16_t>(hi) << 8) |
                                         static_cast<std::uint16_t>(lo));
    };

    out_data.accel_x = to_int16(buffer[0], buffer[1]);
    out_data.accel_y = to_int16(buffer[2], buffer[3]);
    out_data.accel_z = to_int16(buffer[4], buffer[5]);

    out_data.gyro_x  = to_int16(buffer[6], buffer[7]);
    out_data.gyro_y  = to_int16(buffer[8], buffer[9]);
    out_data.gyro_z  = to_int16(buffer[10], buffer[11]);

    return true;
}

bool ICM20948_Driver::write_register(std::uint8_t bank, std::uint8_t reg, std::uint8_t value) {
    if (!select_bank(bank)) {
        return false;
    }

    std::uint8_t tx_buf[2] = { static_cast<std::uint8_t>(reg & 0x7F), value };
    spi_.select();
    spi_.write(std::span<const std::uint8_t>(tx_buf, 2));
    spi_.deselect();
    return true;
}

bool ICM20948_Driver::read_registers(std::uint8_t bank, std::uint8_t reg,
                                     std::span<std::uint8_t> buffer) {
    if (!select_bank(bank)) {
        return false;
    }

    std::array<std::uint8_t, 1> tx{};
    tx[0] = static_cast<std::uint8_t>(reg | 0x80U);

    spi_.select();
    std::array<std::uint8_t, 1> rx_dummy{};
    spi_.transfer(std::span<const std::uint8_t>(tx),
                  std::span<std::uint8_t>(rx_dummy));

    std::array<std::uint8_t, 32> rx_buf{};
    auto rx_span = std::span<std::uint8_t>(rx_buf).first(buffer.size());
    std::array<std::uint8_t, 32> tx_zeros{};
    auto tx_span = std::span<const std::uint8_t>(tx_zeros).first(buffer.size());
    spi_.transfer(tx_span, rx_span);
    spi_.deselect();

    for (std::size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] = rx_span[i];
    }

    return true;
}

bool ICM20948_Driver::select_bank(std::uint8_t bank) {
    return write_register(0, REG_BANK_SEL, bank);
}

bool ICM20948_Driver::configure_power() {
    // Minimal: clear sleep bit
    return write_register(BANK_0, REG_PWR_MGMT_1, 0x01);
}

bool ICM20948_Driver::configure_accel_gyro_4khz() {
    // Placeholder: actual values depend on full-scale and filter config
    if (!write_register(BANK_2, REG_GYRO_SMPLRT_DIV, 0x00)) {
        return false;
    }
    if (!write_register(BANK_2, REG_ACCEL_SMPLRT_DIV_1, 0x00)) {
        return false;
    }
    if (!write_register(BANK_2, REG_ACCEL_SMPLRT_DIV_2, 0x00)) {
        return false;
    }
    return true;
}

} // namespace aerocam
