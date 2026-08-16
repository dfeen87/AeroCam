# AeroCam v2 Architecture Specification

## Overview

AeroCam v2 transforms the AeroCam framework into a dual-domain system:
1. **Real-time MCU Drone & Gimbal Stabilization Firmware (Bare-metal ARM Cortex-M):** Operates on 1 kHz control loops with ICM-20948 IMU telemetry, Extended Kalman Filtering (EKF), and Gaussian decay motor outputs (Vec3 gimbal / Vec4 quadcopter propulsion).
2. **Post-Processing GoPro & Action Camera Motion Deblurring Engine:** C++ standalone library and OpenFX plugin architecture for video post-processing host applications (DaVinci Resolve, Premiere Pro via OFX bridge, Gyroflow).

---

## Dual-Domain Pipeline Architecture

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

## Key Components

### 1. EKF Propulsion & Motion Vector Prediction
- **State Vector:** Position $\mathbf{p}$, Velocity $\mathbf{v}$, Orientation Quaternion $\mathbf{q}$.
- **Drone Propulsion Output:** $\mathbf{m} = \begin{bmatrix} \tau_x & \tau_y & \tau_z & T \end{bmatrix}^T$ where $\tau$ represents attitude control torques and $T$ represents vertical thrust command.
- **RK4 Numerical Integration:** Provides 4th-order Runge-Kutta prediction steps for dynamic stability.

### 2. Telemetry Ingestion Engine
- **Supported Formats:**
  - GoPro GPMF KLV binary data streams (`GYRO`, `ACCL` blocks)
  - Gyroflow `.gyro` CSV logs (`t,gx,gy,gz,ax,ay,az`)
  - Generic CSV telemetry formats with auto-header detection.

### 3. Motion Deblur & Frame-Warp Engine
- **3x3 Homography Matrix Generation:** Computed from relative quaternion camera delta between un-stabilized frame orientation $\mathbf{q}_{\text{raw}}$ and EKF-smoothed orientation $\mathbf{q}_{\text{smooth}}$:
  $$H = K \cdot R(\mathbf{q}_{\text{rel}}) \cdot K^{-1}$$
- **PSF Deblur Kernel Generator:** Calculates directional motion blur radius in pixels:
  $$r_{\text{blur}} = \sqrt{(\omega_y \cdot f_x \cdot t_{\text{exp}})^2 + (\omega_x \cdot f_y \cdot t_{\text{exp}})^2}$$

### 4. Ultra-HD Zero-Blur Mode
Internal maximum reconstruction setting applying Gaussian decay gain filtering to predicted motion blur radii, suppressing high-frequency ringing while maintaining edge micro-contrast.
