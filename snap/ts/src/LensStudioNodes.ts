import { MotionEventBus } from "./MotionEventBus";
import { MotionEventListener, HeadGesture } from "./types";

export class OnTiltNode {
  private bus = MotionEventBus.getInstance();

  public bind(callback: (angle: number, direction: "left" | "right" | "up" | "down") => void): () => void {
    return this.bus.subscribe("tilt", (payload) => {
      callback(payload.angle, payload.direction);
    });
  }
}

export class OnSpinNode {
  private bus = MotionEventBus.getInstance();

  public bind(callback: (angularSpeed: number, axis: "yaw" | "pitch" | "roll") => void): () => void {
    return this.bus.subscribe("spin", (payload) => {
      callback(payload.angularSpeed, payload.axis);
    });
  }
}

export class OnVelocitySpikeNode {
  private bus = MotionEventBus.getInstance();

  public bind(callback: (magnitude: number) => void): () => void {
    return this.bus.subscribe("velocity_spike", (payload) => {
      callback(payload.magnitude);
    });
  }
}

export class OnTrajectoryCurveNode {
  private bus = MotionEventBus.getInstance();

  public bind(callback: (curvatureRadius: number) => void): () => void {
    return this.bus.subscribe("trajectory_curve", (payload) => {
      callback(payload.curvatureRadius);
    });
  }
}

export class OnGestureNode {
  private bus = MotionEventBus.getInstance();

  public bind(callback: (gesture: HeadGesture, confidence: number) => void): () => void {
    return this.bus.subscribe("gesture", (payload) => {
      callback(payload.gesture, payload.confidence);
    });
  }
}
