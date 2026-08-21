import { UnifiedMotionState, Vector3D } from "./types";
import { MotionEventBus } from "./MotionEventBus";

export class AeroCoreService {
  private static instance: AeroCoreService;
  private eventBus: MotionEventBus;
  private currentState: UnifiedMotionState;
  private lastVelocityMagnitude: number = 0;

  private constructor() {
    this.eventBus = MotionEventBus.getInstance();
    this.currentState = this.createInitialState();
  }

  public static getInstance(): AeroCoreService {
    if (!AeroCoreService.instance) {
      AeroCoreService.instance = new AeroCoreService();
    }
    return AeroCoreService.instance;
  }

  private createInitialState(): UnifiedMotionState {
    return {
      timestampNs: 0,
      motionFrame: {
        timestampNs: 0,
        globalDx: 0,
        globalDy: 0,
        rotationalDelta: 0,
        gridVectors: []
      },
      stabilityScore: 1.0,
      headPose: {
        timestampNs: 0,
        orientation: { w: 1, x: 0, y: 0, z: 0 },
        angularVelocity: { x: 0, y: 0, z: 0 },
        linearAcceleration: { x: 0, y: 0, z: 0 },
        detectedGesture: "NONE",
        gestureConfidence: 0,
        arAlignmentCorrection: { w: 1, x: 0, y: 0, z: 0 }
      },
      spotlightMetrics: {
        temporalCoherenceScore: 1.0,
        sceneStabilityScore: 1.0,
        overallQualityIndex: 1.0,
        passesSpotlightThreshold: true
      },
      homographyWarpMatrix: [1, 0, 0, 0, 1, 0, 0, 0, 1]
    };
  }

  public ingestMotionState(partialState: Partial<UnifiedMotionState>): void {
    this.currentState = { ...this.currentState, ...partialState };
    this.evaluateAndPublishEvents();
  }

  public getState(): UnifiedMotionState {
    return { ...this.currentState };
  }

  private evaluateAndPublishEvents(): void {
    const state = this.currentState;

    // 1. Evaluate Tilt Event
    const pitch = state.headPose.angularVelocity.x;
    const roll = state.headPose.angularVelocity.z;
    if (Math.abs(roll) > 1.2) {
      this.eventBus.publish("tilt", {
        angle: roll,
        direction: roll > 0 ? "right" : "left"
      });
    } else if (Math.abs(pitch) > 1.2) {
      this.eventBus.publish("tilt", {
        angle: pitch,
        direction: pitch > 0 ? "up" : "down"
      });
    }

    // 2. Evaluate Spin Event
    const yawRate = state.headPose.angularVelocity.y;
    if (Math.abs(yawRate) > 2.0) {
      this.eventBus.publish("spin", {
        angularSpeed: Math.abs(yawRate),
        axis: "yaw"
      });
    }

    // 3. Evaluate Velocity Spike Event
    const velMag = Math.sqrt(
      state.motionFrame.globalDx * state.motionFrame.globalDx +
      state.motionFrame.globalDy * state.motionFrame.globalDy
    );
    const spikeDelta = Math.abs(velMag - this.lastVelocityMagnitude);
    if (spikeDelta > 0.08) {
      this.eventBus.publish("velocity_spike", {
        magnitude: velMag,
        thresholdExceeded: spikeDelta
      });
    }
    this.lastVelocityMagnitude = velMag;

    // 4. Evaluate Gesture Event
    if (state.headPose.detectedGesture !== "NONE" && state.headPose.gestureConfidence > 0.8) {
      this.eventBus.publish("gesture", {
        gesture: state.headPose.detectedGesture,
        confidence: state.headPose.gestureConfidence
      });
    }

    // 5. Evaluate Trajectory Curve Event
    if (Math.abs(state.motionFrame.rotationalDelta) > 0.15) {
      const radius = 1.0 / (Math.abs(state.motionFrame.rotationalDelta) + 1e-5);
      const predictedNext: Vector3D = {
        x: state.motionFrame.globalDx * 2,
        y: state.motionFrame.globalDy * 2,
        z: 0
      };
      this.eventBus.publish("trajectory_curve", {
        curvatureRadius: radius,
        predictedNextPosition: predictedNext
      });
    }

    // 6. Stability change hint event
    let hint = "steady";
    if (state.stabilityScore < 0.4) {
      hint = "Hold device steady for best quality";
    } else if (state.motionFrame.globalDx < -0.05) {
      hint = "Pan slowly to the right";
    } else if (state.motionFrame.globalDx > 0.05) {
      hint = "Pan slowly to the left";
    }

    this.eventBus.publish("stability_change", {
      stabilityScore: state.stabilityScore,
      hintMessage: hint
    });
  }
}
