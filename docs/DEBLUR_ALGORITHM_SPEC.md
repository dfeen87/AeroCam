# IMU-Driven Motion Deblur Algorithm Specification

## Mathematical Foundation

When an action camera (e.g., GoPro) experiences high-frequency angular vibration or swift rotational movements during a video frame exposure period $t_{\text{exp}}$, the resulting image exhibits directional motion blur. Traditional blind deconvolution relies solely on image pixel heuristics, which are prone to artifacts and computationally expensive.

AeroCam’s **IMU-Driven Deblur Algorithm** leverages high-rate gyro telemetry to deterministically calculate the camera’s exact angular velocity vector $\boldsymbol{\omega} = [\omega_x, \omega_y, \omega_z]^T$ during frame integration, rendering the motion point spread function (PSF) explicitly solvable.

---

## Algorithm Steps

### Step 1: Angular Velocity to Pixel Translation Mapping
Given camera focal lengths $(f_x, f_y)$ in pixels and frame exposure time $t_{\text{exp}}$ in seconds:
- Horizontal pixel displacement $d_x$:
  $$d_x = \omega_y \cdot f_x \cdot t_{\text{exp}}$$
- Vertical pixel displacement $d_y$:
  $$d_y = \omega_x \cdot f_y \cdot t_{\text{exp}}$$

### Step 2: Point Spread Function (PSF) Kernel Generation
The motion blur radius $r$ and angular direction $\theta$ are given by:
$$r = \sqrt{d_x^2 + d_y^2}$$
$$\theta = \text{atan2}(d_y, d_x)$$

For a kernel size $N \times N$, each spatial coordinate $(x, y)$ relative to the kernel center is projected onto parallel ($p_{\parallel}$) and perpendicular ($p_{\perp}$) axes along $\theta$:
$$p_{\parallel} = x \cos\theta + y \sin\theta$$
$$p_{\perp} = -x \sin\theta + y \cos\theta$$

The unnormalized kernel weight $W(x, y)$ is calculated via anisotropic Gaussian decay:
$$W(x, y) = \exp\left( -\frac{1}{2} \left[ \frac{p_{\parallel}^2}{\sigma_{\parallel}^2} + \frac{p_{\perp}^2}{\sigma_{\perp}^2} \right] \right)$$
where $\sigma_{\parallel} = \max(1.0, 0.5 \cdot r)$ and $\sigma_{\perp} = 0.8$.

### Step 3: Ultra-HD Zero-Blur Mode Reconstruction
In **Ultra-HD Zero-Blur Mode**, the engine calculates dynamic reconstruction decay gain $G(r)$ using AeroCam’s Gaussian decay equation:
$$G(r) = \exp\left(-\frac{r^2}{2\sigma_{\text{UHD}}^2}\right)$$
This gain is applied during frequency-domain deconvolution (Wiener filter / Richardson-Lucy iterations) to dynamically regulate restoration strength:
$$\hat{I}(u, v) = \frac{H^*(u, v)}{|H(u, v)|^2 + \frac{\lambda}{G(r)}} \cdot I(u, v)$$
where $I(u, v)$ is the observed blurry frame spectrum and $\hat{I}(u, v)$ is the reconstructed zero-blur frame spectrum.
