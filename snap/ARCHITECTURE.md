# AeroCam → Snap Motion Intelligence Architecture

## Subsystem Architecture & System Design

```
+---------------------------------------------------------------------------------------------------+
|                                     SNAP MOTION INTELLIGENCE                                      |
+---------------------------------------------------------------------------------------------------+
|                                                                                                   |
|  +---------------------------+  +---------------------------+  +-------------------------------+  |
|  |     CAMERA HARDWARE       |  |      SPECTACLES IMU       |  |      LENS STUDIO / CLIENT      |  |
|  |   Frame Buffers & Meta    |  |  Gyro / Accel @ 1000Hz    |  |     Scripting Environment     |  |
|  +-------------+-------------+  +-------------+-------------+  +---------------+---------------+  |
|                |                              |                                |                  |
|                v                              v                                v                  |
|  ==============================================================================================  |
|                                    C++ / HARDWARE ACCELERATION LAYER                              |
|  ==============================================================================================  |
|  +---------------------------+  +---------------------------+  +-------------------------------+  |
|  | MotionVectorExtractor     |  | SpectaclesImuFusion       |  | SpotlightMotionQualityIndex   |  |
|  | (Pyramid Lucas-Kanade OF) |  | (EKF + Gesture Recog)     |  | (Coherence + Stability Score) |  |
|  +-------------+-------------+  +-------------+-------------+  +---------------+---------------+  |
|                |                              |                                |                  |
|                +------------------------------+--------------------------------+                  |
|                                               |                                                   |
|                                               v                                                   |
|                               +-------------------------------+                                   |
|                               | GPU Warp Shader (GLSL)        |                                   |
|                               | Real-time 3x3 Homography      |                                   |
|                               +---------------+---------------+                                   |
|                                               |                                                   |
|  ==============================================================================================  |
|                                  TYPESCRIPT / HIGH-LEVEL ENGINE LAYER                             |
|  ==============================================================================================  |
|                                               v                                                   |
|                               +-------------------------------+                                   |
|                               | AeroCore Service              |                                   |
|                               | (Unified Motion State & Bus)   |                                   |
|                               +---------------+---------------+                                   |
|                                               |                                                   |
|         +-----------------------+-------------+-------------+-----------------------+             |
|         |                       |                           |                       |             |
|         v                       v                           v                       v             |
|  +--------------+     +-------------------+       +-------------------+   +--------------------+  |
|  | MotionEvent  |     | MotionChallenge   |       | MotionMood        |   | CaptureHintService |  |
|  | Bus          |     | Engine            |       | Engine            |   | UI Guidance Hints  |  |
|  +------+-------+     +---------+---------+       +---------+---------+   +--------------------+  |
|         |                       |                           |                                     |
|         v                       v                           v                                     |
|  +--------------+     +-------------------+       +-------------------+                           |
|  | Lens Studio  |     | Social Mini-Games |       | Dynamic Color LUT |                           |
|  | Event Nodes  |     | & Rewards         |       | & AI Prompt Gen   |                           |
|  +--------------+     +-------------------+       +-------------------+                           |
|                                                                                                   |
+---------------------------------------------------------------------------------------------------+
```

---

## 1. Capabilities Overview

1. **Motion-Stabilized Capture Pipeline:**
   - Real-time sparse optical flow vector extraction.
   - Micro-corrections using 3x3 GPU homography warp shader.
   - UI Capture Hints: `steady`, `pan left`, `pan right`, `tilt up`, `tilt down`, `good lighting`.

2. **Trajectory-Aware AR Lens Engine:**
   - `MotionEventBus` publishing rotational velocity, acceleration, trajectory curvature, and spikes.
   - Lens Studio nodes: `OnTilt`, `OnSpin`, `OnVelocitySpike`, `OnTrajectoryCurve`.
   - Physics-reactive AR behaviors (particle swirls, character dodge).

3. **Motion-Challenge Social Mini-Games:**
   - `MotionChallengeEngine` with pre-built challenge evaluation ("Hold steady for 5s", "Trace shape", "Perform 180° spin").
   - Score calculations, reward unlock triggers, and Lens activation events.

4. **Spotlight Motion-Coherence Ranking Module:**
   - `MotionCoherenceScore`: Temporal optical vector smoothness.
   - `SceneStabilityScore`: AeroCam EKF stability metrics.
   - `SpotlightMotionQualityIndex`: Combined quality rating for Spotlight video ranking.

5. **AI-Driven Creative Moods Based on Motion:**
   - Dynamic motion pattern mapping to LUT color grading profiles (e.g. high dynamic motion -> neon cyber gradient).
   - `MotionMoodEngine` generating creative text prompts for AI Lens generation based on movement vibe.

6. **Spectacles Motion-Intelligence Layer:**
   - Sensor fusion of head IMU (gyro + accel) and camera optical flow.
   - Real-time gesture recognition (`Nod`, `Shake`, `Tilt`, `Lean`).
   - AR alignment stabilization for optical see-through display.

7. **Lens Studio Creator Extensions (SDK):**
   - Developer API exposed via `@snap/aerocam-motion-intelligence`:
     - `MotionVectorAPI`
     - `TrajectoryPredictor`
     - `StabilityScoreProvider`
     - `MotionReactiveAnimation`
