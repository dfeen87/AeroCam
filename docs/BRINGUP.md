# AeroCam Firmware Bring-Up Guide

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
**© 2026 Don Michael Feeney Jr**

> [!IMPORTANT]
> **Prerequisites Checklist**
> - Hardware Debugger: ST-Link V2/V3 or J-Link.
> - Analysis Tools: Logic Analyzer (e.g., Saleae or generic 24MHz clone) or Oscilloscope.
> - Software: CMake >= 3.20, ARM GNU Toolchain (`arm-none-eabi-gcc`).

---

## 1. Hardware Wiring

### ICM-20948 (SPI Mode)

| IMU Pin | STM32 Pin | Notes |
| :--- | :--- | :--- |
| **VCC** | `3.3V` | **Do NOT use 5V logic.** |
| **GND** | `GND` | Common ground required. |
| **SCL / SCLK**| `SPI1_SCK` | 10k pull-down recommended. |
| **SDA / SDI** | `SPI1_MOSI` | Push-pull. |
| **ADO / SDO** | `SPI1_MISO` | Floating OK. |
| **CS** | `GPIO` (e.g., `PA4`) | Active low. |

> [!TIP]
> **Hardware Best Practices:**
> * Place a `100nF` decoupling capacitor as close to the IMU as possible.
> * Keep SPI traces ultra-short. Use shielded twisted-pair wire if the run is > 10cm.

## 2. Motor Driver PWM Wiring

| Axis | STM32 Pin | Timer Channel |
| :--- | :--- | :--- |
| **Roll** | e.g., `PA8` | `TIM1_CH1` |
| **Pitch** | e.g., `PA9` | `TIM1_CH2` |
| **Yaw** | e.g., `PA10`| `TIM1_CH3` |

* **PWM frequency:** 200–400 Hz
* **Pulse range:** 1000–2000 µs

---

## 3. Clocking Requirements & Latency

| Architecture | External Crystal | SYSCLK | Bus Targets | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **STM32F4** | 8 MHz | 168 MHz | APB1: 42 MHz, APB2: 84 MHz | EKF Update: < 50 µs |
| **STM32F7** | 8 MHz | 216 MHz | Flash Latency: 7 WS | |
| **STM32H7** | 25 MHz | 400 MHz | Voltage scaling: VOS1 | **EKF Update: < 20 µs**. Watch D-Cache! |

### Control Loop Timing (Total Budget: < 200 µs)
* **1 kHz Control Loop:** `TIM2` configured for 1 ms period. ISR sets `g_tick = true`.
* **IMU ODR:** ICM-20948 configured for **4 kHz**. SPI burst read every 1 ms via DMA.
* **SPI DMA transfer:** ~20–40 µs.

---

## 4. Pre-Flight Bring-Up Checklist

- [ ] **Step 1: Flash Firmware**
  ```bash
  st-flash write AeroCam.bin 0x08000000

* [ ] **Step 2: Verify SWD Interface**
Confirm `SWDIO`, `SWCLK`, `NRST`, and `GND` are solid. You should be able to step through `main()`.
* [ ] **Step 3: Verify SPI & IMU (`WHO_AM_I`)**
* Confirm `CS` toggles active-low.
* Confirm SPI clock is present.
* Read `WHO_AM_I`. **Expected value: `0xEA**`.


* [ ] **Step 4: Check PWM Outputs**
Use an oscilloscope to verify 1–2 ms pulses at 200–400 Hz before connecting motors.
* [ ] **Step 5: First Motion Test**
Run stabilizer in `FILM_MODE`. Verify smooth dampening. Switch to `AERO_MODE` to test aggressive response.

---

## 5. Troubleshooting the SPI Bus

If Step 3 fails and you do not read `0xEA` from the `WHO_AM_I` register:

* **Reads `0x00`:** The SPI bus is dead. Check your 3.3V power, ensure common ground, and verify the STM32 PLL clock is actually running.
* **Reads `0xFF`:** Your `MISO` line is floating high. Check the connection between the IMU's `SDO` and the STM32's `MISO` pin.
* **Garbage Data:** Check your SPI polarity and phase (`CPOL` / `CPHA`). The ICM-20948 requires SPI Mode 3 (CPOL=1, CPHA=1) or Mode 0 (CPOL=0, CPHA=0).

---

> [!WARNING]
> **Safety Architecture Notes**
> 1. **ZERO Dynamic Memory:** Do not use `malloc` or `new` anywhere in the firmware. All buffers must be static or stack-allocated.
> 2. **Watchdog:** Enable the Independent Watchdog Timer (IWDG) for field reliability.
> 3. **Orientation:** Ensure IMU physical orientation matches the quaternion matrix before powering the drive motors.
