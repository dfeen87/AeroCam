"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.CaptureHintService = void 0;
const AeroCoreService_1 = require("./AeroCoreService");
class CaptureHintService {
    aeroCore;
    currentHint = {
        status: "steady",
        message: "Camera steady",
        confidence: 1.0
    };
    constructor() {
        this.aeroCore = AeroCoreService_1.AeroCoreService.getInstance();
    }
    getHint() {
        const state = this.aeroCore.getState();
        const stability = state.stabilityScore;
        const motion = state.motionFrame;
        if (stability > 0.8) {
            this.currentHint = {
                status: "steady",
                message: "Great lighting & camera stability!",
                confidence: stability
            };
        }
        else if (stability > 0.5) {
            if (motion.globalDx < -0.04) {
                this.currentHint = {
                    status: "moving",
                    message: "Pan left slowly",
                    confidence: 0.75
                };
            }
            else if (motion.globalDx > 0.04) {
                this.currentHint = {
                    status: "moving",
                    message: "Pan right slowly",
                    confidence: 0.75
                };
            }
            else {
                this.currentHint = {
                    status: "moving",
                    message: "Keep motion smooth",
                    confidence: 0.70
                };
            }
        }
        else {
            this.currentHint = {
                status: "unstable",
                message: "Hold device steady",
                confidence: 1.0 - stability
            };
        }
        return this.currentHint;
    }
}
exports.CaptureHintService = CaptureHintService;
//# sourceMappingURL=CaptureHintService.js.map