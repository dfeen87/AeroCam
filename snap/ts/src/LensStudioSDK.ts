import { AeroCoreService } from "./AeroCoreService";
import { MotionVectorFrame, Vector3D } from "./types";

export class MotionVectorAPI {
  private aeroCore = AeroCoreService.getInstance();

  public getCurrentFrame(): MotionVectorFrame {
    return this.aeroCore.getState().motionFrame;
  }

  public getGlobalDisplacement(): { dx: number; dy: number } {
    const frame = this.getCurrentFrame();
    return { dx: frame.globalDx, dy: frame.globalDy };
  }
}

export class TrajectoryPredictor {
  private aeroCore = AeroCoreService.getInstance();

  public predictNextPosition(timeStepSec: number): Vector3D {
    const state = this.aeroCore.getState();
    const velX = state.motionFrame.globalDx;
    const velY = state.motionFrame.globalDy;
    const accelX = state.headPose.linearAcceleration.x;
    const accelY = state.headPose.linearAcceleration.y;

    return {
      x: velX * timeStepSec + 0.5 * accelX * timeStepSec * timeStepSec,
      y: velY * timeStepSec + 0.5 * accelY * timeStepSec * timeStepSec,
      z: 0
    };
  }
}

export class StabilityScoreProvider {
  private aeroCore = AeroCoreService.getInstance();

  public getStabilityScore(): number {
    return this.aeroCore.getState().stabilityScore;
  }

  public isStable(threshold: number = 0.75): boolean {
    return this.getStabilityScore() >= threshold;
  }
}

export class MotionReactiveAnimation {
  private aeroCore = AeroCoreService.getInstance();

  public calculateParticleSwirlIntensity(): number {
    const state = this.aeroCore.getState();
    const spinRate = Math.abs(state.headPose.angularVelocity.y);
    return Math.min(1.0, spinRate / 3.0);
  }

  public calculateDodgeOffset(): Vector3D {
    const state = this.aeroCore.getState();
    return {
      x: state.motionFrame.globalDx * 2.5,
      y: state.motionFrame.globalDy * 2.5,
      z: 0
    };
  }
}
