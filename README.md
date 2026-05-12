# AeroCam

**A bare-metal C++ framework targeting ARM Cortex-M microcontrollers for aerospace propulsion math and predictive cinematic stabilization.**

Powered by the AI-Lex Energy Equation and ICM-20948 telemetry, AeroCam delivers ultra-smooth, mathematically rigorous tracking for the open-source film community.

[![Build Status](https://github.com/yourusername/AeroCam/actions/workflows/aerocam-ci.yml/badge.svg)](https://github.com/yourusername/AeroCam/actions/workflows/aerocam-ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=flat&logo=cmake&logoColor=white)](https://cmake.org/)
[![ARM GCC](https://img.shields.io/badge/ARM_GCC-blue.svg)](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain)

---

## 📖 Overview

AeroCam is a mathematically rigorous, predictive cinematic stabilization framework for bare-metal ARM Cortex-M microcontrollers. It seamlessly translates aerospace propulsion math into ultra-smooth camera tracking, utilizing real-time telemetry from the ICM-20948 IMU. By employing an Extended Kalman Filter (EKF) or Complementary Filter alongside Gaussian decay smoothing, it enables cinematographers to achieve unparalleled stabilization precision. Designed to be robust and lightweight, AeroCam relies entirely on static memory allocation, ensuring predictability and safety in mission-critical deployments.

---

## 🏗️ High-Level Architecture

```text
+-------------------+       +-----------------------+       +-------------------+
|                   |       |                       |       |                   |
|   ICM-20948 IMU   |       |    AeroCam Firmware   |       |   Motor Drivers   |
| (Accel/Gyro/Mag)  +------>|    (ARM Cortex-M)     +------>|    (Roll/Pitch/   |
|                   |  SPI  |                       |  PWM  |       Yaw)        |
+-------------------+       +-----------+-----------+       +-------------------+
                                        |
                                        v
                            +-----------------------+
                            |                       |
                            |   1 kHz Control Loop  |
                            |                       |
                            |  - EKF/Complementary  |
                            |  - Quaternion Math    |
                            |  - Gaussian Decay     |
                            |                       |
                            +-----------------------+
```

---

## 🧮 Math Section

AeroCam implements advanced mathematics to deliver pristine stabilization:

- **Quaternion Math:** Avoids gimbal lock and ensures smooth interpolations for orientation calculations.
- **Sensor Fusion (EKF / Complementary Filter):** Merges high-frequency gyro data with stable, low-frequency accelerometer and magnetometer data.
- **Gaussian Decay:** Applies a bell-curve decay filter to predictive trajectories, ensuring cinematic damping on sudden stops and starts.
- **Control Loop Timing (1 kHz):** Maintains a deterministic 1 ms execution period for maximum responsiveness and real-time stability, while polling the IMU at up to 4 kHz.

---

## ⚙️ Hardware Integration

### Microcontroller Support
AeroCam natively targets the STM32 family of ARM Cortex-M microcontrollers:
- **STM32F4:** 168 MHz SYSCLK (8 MHz external crystal)
- **STM32F7:** 216 MHz SYSCLK (8 MHz external crystal)
- **STM32H7:** 400 MHz SYSCLK (25 MHz external crystal)

### ICM-20948 SPI Wiring
Communicate with the ICM-20948 IMU via SPI.

| IMU Pin | STM32 Pin | Notes |
|---------|-----------|-------|
| VCC     | 3.3V      | Do NOT use 5V |
| GND     | GND       | Common ground required |
| SCL/SCLK| SPI1_SCK  | 10k pull-down recommended |
| SDA/SDI | SPI1_MOSI | Push-pull |
| ADO/SDO | SPI1_MISO | Floating OK |
| CS      | GPIO      | Active low (PA4 recommended) |

### PWM Motor Outputs
Configure timers for 200–400 Hz output with a 1000–2000 µs pulse range.

| Axis  | STM32 Pin | Timer Channel |
|-------|-----------|---------------|
| Roll  | PA8       | TIM1_CH1      |
| Pitch | PA9       | TIM1_CH2      |
| Yaw   | PA10      | TIM1_CH3      |

---

## 📂 Repository Structure

```text
.
├── AeroCam.ld
├── CMakeLists.txt
├── Drivers
│   ├── STM32F4xx_HAL_Driver
│   │   └── Inc
│   │       └── stm32f4xx_hal.h
│   ├── STM32F7xx_HAL_Driver
│   │   └── Inc
│   │       └── stm32f7xx_hal.h
│   └── STM32H7xx_HAL_Driver
│       └── Inc
│           └── stm32h7xx_hal.h
├── LICENSE
├── README.md
├── cmake
│   └── arm-gcc-toolchain.cmake
├── docs
│   └── BRINGUP.md
├── include
│   ├── aerocam_stabilizer.hpp
│   ├── gimbal_mixer.hpp
│   ├── icm20948_driver.hpp
│   ├── mcu_hal.hpp
│   ├── quaternion.hpp
│   └── spi_dma.hpp
└── src
    ├── aerocam_stabilizer.cpp
    ├── icm20948_driver.cpp
    ├── main.cpp
    ├── quaternion.cpp
    └── startup_aerocam.s

12 directories, 20 files
```

---

## 🛠️ Build Instructions

### Dependencies
Install the ARM GCC toolchain, CMake, and Make:
```bash
sudo apt-get install -y cmake make gcc-arm-none-eabi binutils-arm-none-eabi gdb-multiarch
```

### Local Build
```bash
mkdir build && cd build
cmake -DMCU=<F4|F7|H7> -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-gcc-toolchain.cmake ..
make
```

### Multi-MCU Matrix Build
For building multiple targets:
```bash
# Build for STM32F4
mkdir build-F4 && cd build-F4
cmake -DMCU=F4 -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-gcc-toolchain.cmake ..
make

# Build for STM32H7
cd .. && mkdir build-H7 && cd build-H7
cmake -DMCU=H7 -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-gcc-toolchain.cmake ..
make
```

### CI Pipeline
A GitHub Actions CI workflow is configured (`.github/workflows/aerocam-ci.yml`) to automatically build firmware for `F4`, `F7`, and `H7` targets upon every push and pull request to the `main` branch. It utilizes Ninja for fast builds and uploads the resulting binaries (`.elf`, `.bin`, `.map`) as artifacts.

---

## ⚡ Flashing Instructions

Once built, flash the `.bin` to your board using ST-Link:

```bash
st-flash write build/AeroCam.bin 0x08000000
```

---

## 🚀 Bring-Up Checklist

Refer to [`docs/BRINGUP.md`](docs/BRINGUP.md) for full details.

- [ ] **Flash Firmware:** Verify successful upload via ST-Link.
- [ ] **Verify SWD:** Connect SWDIO, SWCLK, NRST, and GND.
- [ ] **Check IMU Communication:**
    - Confirm CS toggles and SPI clock is active.
    - Read `WHO_AM_I` (expect `0xEA`).
- [ ] **Check PWM Outputs:** Scope the pins to verify 1–2 ms pulses at 200–400 Hz.
- [ ] **First Motion Test:** Test `FILM_MODE` for damping, then `AERO_MODE` for aggressive stabilization.

---

## 📄 License

AeroCam is distributed under the MIT License. See [LICENSE](LICENSE) for more details.

---

## 🙌 Acknowledgements

Special thanks to the following for their assistance in coding and developing this repository:
- Google Jules
- OpenAI Codex
- Microsoft Copilot
