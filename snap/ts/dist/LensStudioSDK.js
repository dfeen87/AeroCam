"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.MotionReactiveAnimation = exports.StabilityScoreProvider = exports.TrajectoryPredictor = exports.MotionVectorAPI = void 0;
const AeroCoreService_1 = require("./AeroCoreService");
class MotionVectorAPI {
    aeroCore = AeroCoreService_1.AeroCoreService.getInstance();
    getCurrentFrame() {
        return this.aeroCore.getState().motionFrame;
    }
    getGlobalDisplacement() {
        const frame = this.getCurrentFrame();
        return { dx: frame.globalDx, dy: frame.globalDy };
    }
}
exports.MotionVectorAPI = MotionVectorAPI;
class TrajectoryPredictor {
    aeroCore = AeroCoreService_1.AeroCoreService.getInstance();
    predictNextPosition(timeStepSec) {
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
exports.TrajectoryPredictor = TrajectoryPredictor;
class StabilityScoreProvider {
    aeroCore = AeroCoreService_1.AeroCoreService.getInstance();
    getStabilityScore() {
        return this.aeroCore.getState().stabilityScore;
    }
    isStable(threshold = 0.75) {
        return this.getStabilityScore() >= threshold;
    }
}
exports.StabilityScoreProvider = StabilityScoreProvider;
class MotionReactiveAnimation {
    aeroCore = AeroCoreService_1.AeroCoreService.getInstance();
    calculateParticleSwirlIntensity() {
        const state = this.aeroCore.getState();
        const spinRate = Math.abs(state.headPose.angularVelocity.y);
        return Math.min(1.0, spinRate / 3.0);
    }
    calculateDodgeOffset() {
        const state = this.aeroCore.getState();
        return {
            x: state.motionFrame.globalDx * 2.5,
            y: state.motionFrame.globalDy * 2.5,
            z: 0
        };
    }
}
exports.MotionReactiveAnimation = MotionReactiveAnimation;
//# sourceMappingURL=LensStudioSDK.js.map