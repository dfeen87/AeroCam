/**
 * Core Motion Intelligence Types and Interfaces for Snap AeroCam
 */
export interface Vector3D {
    x: number;
    y: number;
    z: number;
}
export interface Quaternion {
    w: number;
    x: number;
    y: number;
    z: number;
}
export interface OpticalFlowVector {
    x: number;
    y: number;
    dx: number;
    dy: number;
    confidence: number;
}
export interface MotionVectorFrame {
    timestampNs: number;
    globalDx: number;
    globalDy: number;
    rotationalDelta: number;
    gridVectors: OpticalFlowVector[];
}
export type HeadGesture = "NONE" | "NOD" | "SHAKE" | "TILT_LEFT" | "TILT_RIGHT" | "LEAN_FORWARD" | "LEAN_BACKWARD";
export interface SpectaclesHeadPose {
    timestampNs: number;
    orientation: Quaternion;
    angularVelocity: Vector3D;
    linearAcceleration: Vector3D;
    detectedGesture: HeadGesture;
    gestureConfidence: number;
    arAlignmentCorrection: Quaternion;
}
export interface SpotlightMotionMetrics {
    temporalCoherenceScore: number;
    sceneStabilityScore: number;
    overallQualityIndex: number;
    passesSpotlightThreshold: boolean;
}
export interface UnifiedMotionState {
    timestampNs: number;
    motionFrame: MotionVectorFrame;
    stabilityScore: number;
    headPose: SpectaclesHeadPose;
    spotlightMetrics: SpotlightMotionMetrics;
    homographyWarpMatrix: number[];
}
export type MotionEventType = "tilt" | "spin" | "velocity_spike" | "trajectory_curve" | "gesture" | "stability_change";
export interface MotionEventPayloadMap {
    tilt: {
        angle: number;
        direction: "left" | "right" | "up" | "down";
    };
    spin: {
        angularSpeed: number;
        axis: "yaw" | "pitch" | "roll";
    };
    velocity_spike: {
        magnitude: number;
        thresholdExceeded: number;
    };
    trajectory_curve: {
        curvatureRadius: number;
        predictedNextPosition: Vector3D;
    };
    gesture: {
        gesture: HeadGesture;
        confidence: number;
    };
    stability_change: {
        stabilityScore: number;
        hintMessage: string;
    };
}
export type MotionEventListener<T extends MotionEventType> = (payload: MotionEventPayloadMap[T]) => void;
//# sourceMappingURL=types.d.ts.map