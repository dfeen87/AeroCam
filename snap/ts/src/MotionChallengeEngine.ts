import { AeroCoreService } from "./AeroCoreService";

export type ChallengeType = "HOLD_STEADY" | "TRACE_SHAPE" | "SPIN_180";

export interface ChallengeDefinition {
  id: string;
  type: ChallengeType;
  title: string;
  description: string;
  targetDurationSec: number;
  rewardUnlockId: string;
}

export interface ChallengeProgress {
  challengeId: string;
  completed: boolean;
  score: number; // [0, 100]
  elapsedTimeSec: number;
  rewardUnlocked: boolean;
}

export class MotionChallengeEngine {
  private aeroCore = AeroCoreService.getInstance();
  private activeChallenge: ChallengeDefinition | null = null;
  private startTimeMs: number = 0;
  private accumulatedSteadyTimeMs: number = 0;
  private accumulatedSpinRad: number = 0;

  public startChallenge(challenge: ChallengeDefinition): void {
    this.activeChallenge = challenge;
    this.startTimeMs = Date.now();
    this.accumulatedSteadyTimeMs = 0;
    this.accumulatedSpinRad = 0;
  }

  public update(dtSec: number): ChallengeProgress | null {
    if (!this.activeChallenge) return null;

    const state = this.aeroCore.getState();
    const elapsedSec = (Date.now() - this.startTimeMs) / 1000.0;

    let score = 0;
    let completed = false;

    switch (this.activeChallenge.type) {
      case "HOLD_STEADY": {
        if (state.stabilityScore > 0.8) {
          this.accumulatedSteadyTimeMs += dtSec * 1000;
        }
        const steadySec = this.accumulatedSteadyTimeMs / 1000.0;
        score = Math.min(100, (steadySec / this.activeChallenge.targetDurationSec) * 100);
        completed = steadySec >= this.activeChallenge.targetDurationSec;
        break;
      }

      case "SPIN_180": {
        const yawRate = Math.abs(state.headPose.angularVelocity.y);
        this.accumulatedSpinRad += yawRate * dtSec;
        const degrees = (this.accumulatedSpinRad * 180.0) / Math.PI;
        score = Math.min(100, (degrees / 180.0) * 100);
        completed = degrees >= 180.0;
        break;
      }

      case "TRACE_SHAPE": {
        const motionMag = Math.sqrt(
          state.motionFrame.globalDx * state.motionFrame.globalDx +
          state.motionFrame.globalDy * state.motionFrame.globalDy
        );
        if (motionMag > 0.02) {
          this.accumulatedSteadyTimeMs += dtSec * 1000;
        }
        const traceSec = this.accumulatedSteadyTimeMs / 1000.0;
        score = Math.min(100, (traceSec / this.activeChallenge.targetDurationSec) * 100);
        completed = traceSec >= this.activeChallenge.targetDurationSec;
        break;
      }
    }

    return {
      challengeId: this.activeChallenge.id,
      completed,
      score,
      elapsedTimeSec: elapsedSec,
      rewardUnlocked: completed
    };
  }
}
