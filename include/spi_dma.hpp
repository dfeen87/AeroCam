#ifndef AEROCAM_SPI_DMA_HPP
#define AEROCAM_SPI_DMA_HPP
/*
 * MIT License
 * Copyright (c) 2026 Don Michael Feeney Jr
 */

#include <cstddef>
#include <cstdint>
#include <span>

#include "icm20948_driver.hpp"

#if defined(MCU_F4)
    #include "stm32f4xx_hal.h"
#elif defined(MCU_F7)
    #include "stm32f7xx_hal.h"
#elif defined(MCU_H7)
    #include "stm32h7xx_hal.h"
#else
    #error "No MCU family defined. Define MCU_F4, MCU_F7, or MCU_H7."
#endif

namespace aerocam {

class SpiBus_STM32_DMA final : public SpiBus {
public:
    SpiBus_STM32_DMA(SPI_HandleTypeDef* hspi,
                     GPIO_TypeDef* chip_select_port,
                     std::uint16_t chip_select_pin)
        : hspi_(hspi),
          chip_select_port_(chip_select_port),
          chip_select_pin_(chip_select_pin) {}

    void* operator new(std::size_t) = delete;
    void* operator new[](std::size_t) = delete;
    void select() override {
        HAL_GPIO_WritePin(chip_select_port_, chip_select_pin_, GPIO_PIN_RESET);
    }

    void deselect() override {
        wait_for_dma_completion();
        HAL_GPIO_WritePin(chip_select_port_, chip_select_pin_, GPIO_PIN_SET);
    }

    void transfer(std::span<const std::uint8_t> tx,
                  std::span<std::uint8_t> rx) override {
        if (tx.empty() || tx.size() != rx.size() || tx.size() > max_dma_count) {
            return;
        }

        dma_complete_ = false;
        const HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_DMA(
            hspi_,
            const_cast<std::uint8_t*>(tx.data()),
            rx.data(),
            static_cast<std::uint16_t>(tx.size()));

        if (status != HAL_OK) {
            dma_complete_ = true;
            return;
        }

        wait_for_dma_completion();
    }

    void write(std::span<const std::uint8_t> tx) override {
        if (tx.empty() || tx.size() > max_dma_count) {
            return;
        }

        dma_complete_ = false;
        const HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(
            hspi_,
            const_cast<std::uint8_t*>(tx.data()),
            static_cast<std::uint16_t>(tx.size()));

        if (status != HAL_OK) {
            dma_complete_ = true;
            return;
        }

        wait_for_dma_completion();
    }

    void on_txrx_complete() {
        dma_complete_ = true;
    }

    [[nodiscard]] bool dma_complete() const {
        return dma_complete_;
    }

private:
    void wait_for_dma_completion() const {
        while (!dma_complete_) {
            // DMA completion is reported by on_txrx_complete(), normally from
            // HAL_SPI_TxRxCpltCallback() or HAL_SPI_TxCpltCallback().
        }
    }

    static constexpr std::size_t max_dma_count = 0xFFFFU;

    SPI_HandleTypeDef* hspi_;
    GPIO_TypeDef* chip_select_port_;
    std::uint16_t chip_select_pin_;
    mutable volatile bool dma_complete_ = true;
};

} // namespace aerocam

#endif // AEROCAM_SPI_DMA_HPP
