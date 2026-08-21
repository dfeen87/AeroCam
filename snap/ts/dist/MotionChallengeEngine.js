"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.MotionChallengeEngine = void 0;
const AeroCoreService_1 = require("./AeroCoreService");
class MotionChallengeEngine {
    aeroCore = AeroCoreService_1.AeroCoreService.getInstance();
    activeChallenge = null;
    startTimeMs = 0;
    accumulatedSteadyTimeMs = 0;
    accumulatedSpinRad = 0;
    startChallenge(challenge) {
        this.activeChallenge = challenge;
        this.startTimeMs = Date.now();
        this.accumulatedSteadyTimeMs = 0;
        this.accumulatedSpinRad = 0;
    }
    update(dtSec) {
        if (!this.activeChallenge)
            return null;
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
                const motionMag = Math.sqrt(state.motionFrame.globalDx * state.motionFrame.globalDx +
                    state.motionFrame.globalDy * state.motionFrame.globalDy);
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
exports.MotionChallengeEngine = MotionChallengeEngine;
//# sourceMappingURL=MotionChallengeEngine.js.map