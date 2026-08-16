# AeroCam v3.0.0

**A bare-metal C++ framework for ARM Cortex-M microcontrollers and post-processing OpenFX video stabilization plugins powered by the AI-Lex Energy Equation.**

[![Version](https://img.shields.io/badge/AeroCam-v3.0.0-blue.svg)](#)
[![Build Status](https://github.com/dfeen87/AeroCam/actions/workflows/aerocam-ci.yml/badge.svg)](https://github.com/dfeen87/AeroCam/actions/workflows/aerocam-ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=flat&logo=cmake&logoColor=white)](https://cmake.org/)
[![ARM GCC](https://img.shields.io/badge/ARM_GCC-blue.svg)](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain)
[![OpenFX Standard](https://img.shields.io/badge/OpenFX-v1.4-green.svg)](http://openfx.sourceforge.net/)

---

## Overview

**AeroCam v3.0.0** is an advanced dual-domain system designed for real-time aerospace propulsion stabilization and cinematic video post-processing:

1. **Real-Time Embedded MCU Firmware:** Runs bare-metal on ARM Cortex-M microcontrollers (STM32F4/F7/H7) at 1 kHz control loop frequencies with deterministic static memory allocation. It fuses ICM-20948 IMU telemetry via Extended Kalman Filtering (EKF) to output 3D gimbal motor torque or 4D quadcopter propulsion vectors.
2. **Post-Processing Video Stabilization & Deblur Engine:** C++ standalone library and C-ABI OpenFX plugin for NLE suites (DaVinci Resolve, Adobe Premiere Pro, Final Cut Pro). Ingests telemetry streams (GoPro GPMF binary, Gyroflow, CSV) to perform 3x3 frame-warp homography, trajectory SLERP smoothing, and anisotropic point spread function (PSF) motion deblurring.

---

## Dual-Domain Architecture

```text
========================================================================================
                          REAL-TIME FIRMWARE DOMAIN (MCU)
========================================================================================

 +-------------------+    SPI    +-----------------------+   PWM Motor Cmds   +------------------+
 |                   |---------->|   ICM20948 Driver     |------------------->|  Brushless ESCs  |
 |   ICM-20948 IMU   |           |   & EKF State Estim.  |    (Vec3/Vec4)     |  / Gimbal Motors |
 |                   |<----------| (1 kHz Loop / 4kHz SR)|                    +------------------+
 +-------------------+           +-----------------------+

========================================================================================
                       POST-PROCESSING PLUGIN DOMAIN (C++)
========================================================================================

 +-------------------+           +-----------------------+                    +------------------+
 |  IMU Log Stream   |           |  IMU Telemetry Parser |                    |  Smooth Traject. |
 | (GoPro GPMF / CSV |---------->|  (GoPro GPMF, CSV,    |------------------->|  Quaternion EKF  |
 |  Gyroflow .gyro)  |           |   Gyroflow .gyro)     |                    |  Gaussian Filter |
 +-------------------+           +-----------------------+                    +--------+---------+
                                                                                       |
                                                                                       v
 +-------------------+           +-----------------------+                    +--------+---------+
 | Frame Processing  |<----------| Frame-Warp Generator  |<-------------------| Relative Motion  |
 |  (OpenFX Plugin)  |           | (3x3 Homography H)    |                    | Delta (q_target  |
 +---------+---------+           +-----------------------+                    |  * q_source^-1)  |
           |                                                                  +------------------+
           v
 +-------------------+           +-----------------------+
 | Ultra-HD Zero-Blur|           | Point Spread Function |
 |  Reconstruction   |<----------| Deblur Kernel Generator
 |     Output        |           | (PSF / Motion Angle)  |
 +-------------------+           +-----------------------+
```

---

## The AILEE Mathematics

AeroCam’s core engine is anchored by the **AI-Lex Energy Equation (AILEE)**, a mathematical framework that unifies energy decay damping, sensor fusion, non-linear quaternion kinematics, projective geometry, and frequency-domain restoration.

### 1. AI-Lex Energy Equation (AILEE) & Gaussian Damping
The AILEE formulation computes energy decay damping to smooth aggressive velocity deviations while preventing motor saturation or visual boundary artifacts.

Given a velocity tracking error vector $e = \mathbf{v}_{\text{est}} - \mathbf{v}_{\text{desired}}$, the AILEE Gaussian decay scalar $G(e)$ is defined as:
$$G(e) = \exp\left(-\frac{\|e\|^2}{2\sigma^2}\right)$$

The resulting damped correction command vector $\mathbf{u}_{\text{cmd}}$ is given by:
$$\mathbf{u}_{\text{cmd}} = -G(e) \cdot e$$

Where $\sigma$ is configured dynamically according to the active operating profile:
- **`FILM_MODE` ($\sigma = 0.8$):** Smooth, cinematic damping for slow camera moves.
- **`DRONE_MODE` ($\sigma = 0.5$):** Balanced response for multirotor flight stability and altitude hold.
- **`AERO_MODE` ($\sigma = 0.2$):** Aggressive energy response for high-speed aerobatic maneuvers.

In post-processing, the **Ultra-HD Zero-Blur Mode** uses the AILEE decay equation to compute dynamic reconstruction gain $G(r)$ from the motion blur radius $r$:
$$G(r) = \exp\left(-\frac{r^2}{2\sigma_{\text{UHD}}^2}\right)$$

This gain regulates Wiener frequency deconvolution to eliminate motion blur without ringing artifacts:
$$\hat{I}(u, v) = \frac{H^*(u, v)}{|H(u, v)|^2 + \frac{\lambda}{G(r)}} \cdot I(u, v)$$

### 2. Extended Kalman Filtering (EKF) & RK4 Integration
The state vector $\mathbf{x}$ consists of position $\mathbf{p}$, velocity $\mathbf{v}$, and orientation quaternion $\mathbf{q}$:
$$\mathbf{x} = \begin{bmatrix} \mathbf{p} & \mathbf{v} & \mathbf{q} \end{bmatrix}^T$$

Prediction uses 4th-order Runge-Kutta (RK4) integration over time step $\Delta t$:
$$k_1 = f(\mathbf{x}_t), \quad k_2 = f\left(\mathbf{x}_t + \frac{\Delta t}{2} k_1\right), \quad k_3 = f\left(\mathbf{x}_t + \frac{\Delta t}{2} k_2\right), \quad k_4 = f(\mathbf{x}_t + \Delta t \, k_3)$$
$$\mathbf{x}_{t+\Delta t} = \mathbf{x}_t + \frac{\Delta t}{6} \left( k_1 + 2k_2 + 2k_3 + k_4 \right)$$

Measurement update computes innovation error $\mathbf{y} = \mathbf{z}_{\text{meas}} - \mathbf{v}_{\text{est}}$ and updates state estimates deterministically.

### 3. Quaternion Kinematics & SLERP Trajectory Damping
Orientations are represented as unit quaternions $\mathbf{q} = [w, x, y, z]^T$. Angular velocity $\boldsymbol{\omega} = [\omega_x, \omega_y, \omega_z]^T$ integrates into orientation via:
$$\mathbf{q}_{k+1} = \mathbf{q}_k \otimes \Delta \mathbf{q}(\boldsymbol{\omega}, \Delta t)$$

Smooth motion trajectory curves are constructed using Spherical Linear Interpolation (SLERP):
$$\text{Slerp}(\mathbf{q}_1, \mathbf{q}_2, \alpha) = \frac{\sin((1-\alpha)\theta)}{\sin\theta} \mathbf{q}_1 + \frac{\sin(\alpha\theta)}{\sin\theta} \mathbf{q}_2, \quad \cos\theta = \mathbf{q}_1 \cdot \mathbf{q}_2$$

### 4. 3x3 Homography Frame-Warp Calculation
Relative orientation change $\mathbf{q}_{\text{rel}}$ between original un-stabilized frame orientation $\mathbf{q}_{\text{raw}}$ and smoothed camera trajectory $\mathbf{q}_{\text{smooth}}$ is calculated as:
$$\mathbf{q}_{\text{rel}} = \mathbf{q}_{\text{smooth}} \otimes \mathbf{q}_{\text{raw}}^{-1}$$

Converting $\mathbf{q}_{\text{rel}}$ into rotation matrix $R(\mathbf{q}_{\text{rel}})$ yields the 3x3 projective homography matrix $H$:
$$H = K \cdot R(\mathbf{q}_{\text{rel}}) \cdot K^{-1}$$
Where $K$ is the intrinsic pinhole camera matrix:
$$K = \begin{bmatrix} f_x & 0 & c_x \\ 0 & f_y & c_y \\ 0 & 0 & 1 \end{bmatrix}$$

### 5. Anisotropic Point Spread Function (PSF) Motion Deblur Kernel
Rotational angular velocity $\boldsymbol{\omega}$ causes spatial pixel displacement during exposure $t_{\text{exp}}$:
$$d_x = \omega_y \cdot f_x \cdot t_{\text{exp}}, \quad d_y = \omega_x \cdot f_y \cdot t_{\text{exp}}$$
$$r = \sqrt{d_x^2 + d_y^2}, \quad \theta = \text{atan2}(d_y, d_x)$$

Spatial coordinates $(x, y)$ projected onto motion axes $(p_{\parallel}, p_{\perp})$ yield anisotropic Gaussian weights:
$$W(x, y) = \exp\left( -\frac{1}{2} \left[ \frac{p_{\parallel}^2}{\sigma_{\parallel}^2} + \frac{p_{\perp}^2}{\sigma_{\perp}^2} \right] \right)$$
Normalized across the $N \times N$ kernel matrix to preserve total image energy.

---

## GoPro Information & Telemetry Ingestion

AeroCam includes a dedicated binary parser for **GoPro Metadata Format (GPMF)**, enabling direct ingestion of embedded telemetry from GoPro action cameras.

### Supported Features & Formats
- **GoPro GPMF Binary Streams:** Extracts `GYRO` (gyroscope angular velocity in rad/s) and `ACCL` (accelerometer linear acceleration in m/s²) KLV payload blocks directly from GoPro `.mp4` video files.
- **GoPro Camera Generations:** Fully compatible with GoPro HERO 8, 9, 10, 11, 12, and 13 action cameras.
- **Gyroflow Integration:** Parses Gyroflow `.gyro` JSON/CSV logs (`t, gx, gy, gz, ax, ay, az`).
- **Generic CSV Ingestion:** Auto-detects comma-separated telemetry headers.

### C++ Code Example
```cpp
#include "openfx_plugin.hpp"
#include <vector>

aerocam::AeroCamVideoStabilizer stabilizer;

// Load GoPro GPMF telemetry binary buffer extracted from MP4 track
std::vector<uint8_t> gpmf_buffer = load_gpmf_track("gopro_hero12_clip.mp4");
stabilizer.load_telemetry_gpmf(gpmf_buffer.data(), gpmf_buffer.size());

// Configure plugin parameters
aerocam::AeroCamPluginParams params;
params.mode = aerocam::StabilizationMode::ULTRA_HD_ZERO_BLUR;
params.smoothness = 0.75f;
params.exposure_time_sec = 0.002f; // 1/500s shutter
stabilizer.set_params(params);

// Process frame at 12.5 seconds
aerocam::Mat3x3 warp_matrix;
aerocam::DeblurKernel deblur_kernel;
stabilizer.process_frame(12.5, warp_matrix, deblur_kernel);
```

---

## YouTube Integration & Video Post-Processing

AeroCam provides post-processing capabilities tailored for YouTube content creators, action vloggers, FPV drone pilots, and cinema channels.

### OpenFX NLE Suite Compatibility
Exposes standard C-ABI OpenFX plugin hooks (`.ofx` bundle) for seamless integration with:
- **DaVinci Resolve / Studio**
- **Adobe Premiere Pro & After Effects** (via OpenFX bridge)
- **Final Cut Pro**
- **Gyroflow Ecosystem**

### YouTube Production Workflows
- **Ultra-HD Zero-Blur Mode:** Restores micro-contrast and removes rotational shutter blur during high-speed pans, maintaining crystal-clear 4K/8K resolution for YouTube video encoding.
- **Crop-to-Fit & Smooth Damping:** Smooths erratic handheld or helmet-mounted action footage into fluid, cinematic motion curves while maximizing usable frame area.
- **High Frame-Rate Support:** Seamlessly handles 60 FPS, 120 FPS, and 240 FPS high-rate action camera footage.

---

## Flying Drone Support

AeroCam features native flying drone and quadcopter support through the `Drone_Stabilizer` engine in the `aerocam_drone` namespace.

### Quadcopter Propulsion Dynamics
Unlike 3-axis camera gimbals that output 3D torque vectors, flying drones require 4D propulsion control vectors combining angular roll/pitch/yaw torques with vertical collective thrust:
$$\mathbf{m} = \begin{bmatrix} \tau_x & \tau_y & \tau_z & T \end{bmatrix}^T$$
- $\tau_x, \tau_y, \tau_z$: Roll, pitch, and yaw torque commands.
- $T$: Collective thrust control ($0.0$ to $1.0$).

### Altitude Hold Mode
In `DRONE_MODE`, `Drone_Stabilizer` executes an Altitude Hold control loop combining EKF vertical velocity estimation with a P-controller and dynamic hover throttle baseline:
$$T = \text{clamp}\left( K_p \cdot (0 - v_{z,\text{est}}) + T_{\text{hover}}, \; 0.0, \; 1.0 \right)$$

### Code Example
```cpp
#include "drone_stabilizer.hpp"

aerocam::ICM20948_Driver imu_driver;
aerocam_drone::Drone_Stabilizer drone_stabilizer(imu_driver);

// Set mode to DRONE_MODE with Altitude Hold
drone_stabilizer.set_mode(aerocam::StabilizerMode::DRONE_MODE);
drone_stabilizer.set_target_altitude(5.0f); // 5 meters

// Run control loop at 1 kHz
drone_stabilizer.update(0.001f);
aerocam::Vec4 motor_cmd = drone_stabilizer.motor_command();
// motor_cmd.x = Roll, motor_cmd.y = Pitch, motor_cmd.z = Yaw, motor_cmd.w = Thrust
```

---

## Hardware Integration

### Microcontroller Support
Targeted for bare-metal ARM Cortex-M microcontrollers:
- **STM32F4:** 168 MHz SYSCLK (8 MHz external crystal)
- **STM32F7:** 216 MHz SYSCLK (8 MHz external crystal)
- **STM32H7:** 400 MHz SYSCLK (25 MHz external crystal)

### ICM-20948 SPI Wiring
Communicates with the ICM-20948 IMU over SPI up to 7 MHz clock speed.

| IMU Pin | STM32 Pin | Notes |
|---------|-----------|-------|
| VCC     | 3.3V      | Do NOT connect to 5V |
| GND     | GND       | Common system ground |
| SCL/SCLK| SPI1_SCK  | 10k pull-down recommended |
| SDA/SDI | SPI1_MOSI | Push-pull output |
| ADO/SDO | SPI1_MISO | High impedance floating OK |
| CS      | GPIO      | Active low chip select (PA4) |

### PWM Motor & ESC Outputs
Configures hardware timers for 200–400 Hz output with a 1000–2000 µs pulse width range.

| Axis / Channel | STM32 Pin | Timer Channel | Application |
|----------------|-----------|---------------|-------------|
| Roll / M1      | PA8       | TIM1_CH1      | Gimbal Roll / Motor 1 |
| Pitch / M2     | PA9       | TIM1_CH2      | Gimbal Pitch / Motor 2 |
| Yaw / M3       | PA10      | TIM1_CH3      | Gimbal Yaw / Motor 3 |
| Thrust / M4    | PA11      | TIM1_CH4      | Quadcopter Thrust / Motor 4 |

---

## Build Instructions

### Dependencies
Install the ARM GCC toolchain, CMake, and build tools:
```bash
sudo apt-get install -y cmake make ninja-build gcc-arm-none-eabi binutils-arm-none-eabi gdb-multiarch
```

### Local Firmware Build
```bash
mkdir build && cd build
cmake -DMCU=<F4|F7|H7> -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-gcc-toolchain.cmake ..
make
```

### Multi-MCU Matrix Build
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

### Native Unit Testing
Run C++20 host unit tests for EKF, Quaternion math, and deblur calculations:
```bash
cmake -S . -B build-tests -G Ninja -DAEROCAM_TESTS=ON
cmake --build build-tests
cd build-tests && ctest --output-on-failure
```

---

## Flashing Instructions

Flash the compiled binary `.bin` to your STM32 target board using ST-Link:

```bash
st-flash write build/AeroCam.bin 0x08000000
```

---

## Bring-Up Checklist

Refer to [`docs/BRINGUP.md`](docs/BRINGUP.md) for step-by-step diagnostic bring-up instructions:

- [ ] **Flash Firmware:** Verify upload completes successfully via ST-Link.
- [ ] **Verify SWD Connections:** Check SWDIO, SWCLK, NRST, and GND pins.
- [ ] **Check IMU Communication:**
    - Confirm CS toggles and SPI clock is active.
    - Read `WHO_AM_I` register (expect `0xEA`).
- [ ] **Check PWM & ESC Outputs:** Measure pulse width (1–2 ms at 200–400 Hz) with an oscilloscope.
- [ ] **First Motion Test:** Verify `FILM_MODE` for damped cinematic moves, `AERO_MODE` for responsive tracking, and `DRONE_MODE` for quadcopter flight torque/thrust outputs.

---

## License

AeroCam is licensed under the MIT License. See [LICENSE](LICENSE) for the full license text.

---

## Acknowledgements

Special thanks to the following for their contributions and collaboration in developing AeroCam:
- Google Jules
- OpenAI Codex
- Microsoft Copilot
