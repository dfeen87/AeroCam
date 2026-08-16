# OpenFX & Gyroflow Plugin Integration Proposal

## Overview

To enable AeroCam’s mathematical stabilization and motion-deblurring engine across popular professional video editing suites (DaVinci Resolve, Adobe Premiere Pro, Final Cut Pro) and open-source ecosystems (Gyroflow), AeroCam exposes a standardized C-ABI OpenFX plugin interface and standalone C++ API.

---

## Target Host Compatibility

| Host Application | Integration Path | Status |
| :--- | :--- | :--- |
| **DaVinci Resolve** | Native OpenFX (`.ofx` bundle) | Supported |
| **Adobe Premiere Pro / After Effects** | OpenFX Bridge / OFX Plugin | Supported |
| **Gyroflow** | Gyroflow Metadata Exporter & Engine API | Supported |
| **Standalone CLI / Custom Apps** | C++ Static/Dynamic Library (`libAeroCamLib`) | Supported |

---

## C-ABI Plugin Specification

The plugin exports standard entry points defined in `include/openfx_plugin.hpp`:

```cpp
extern "C" {
    struct OFXImageBuffer {
        int width;
        int height;
        int row_bytes;
        float* rgba_data;
    };

    int aerocam_ofx_init();
    int aerocam_ofx_process_frame(double time, const OFXImageBuffer* in_img, OFXImageBuffer* out_img, const AeroCamPluginParams* params);
    int aerocam_ofx_cleanup();
}
```

---

## User Control Parameters

1. **Stabilization Mode:**
   - `0`: Off
   - `1`: Smooth Film Mode (Gaussian decay damping)
   - `2`: Aggressive Aero Mode (High responsiveness)
   - `3`: Ultra-HD Zero-Blur Mode (Motion-aware reconstruction)
2. **Smoothness (0.0 to 1.0):** Trajectory smoothing filter strength.
3. **Exposure Time (seconds):** Camera shutter speed (e.g. 0.002s for 1/500s shutter).
4. **Focal Length (pixels):** Optical lens focal length in pixels.
5. **Enable Deblur (Boolean):** Enables point spread function motion deblurring kernel computation.
