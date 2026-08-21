import { MotionVectorFrame, Vector3D } from "./types";
export declare class MotionVectorAPI {
    private aeroCore;
    getCurrentFrame(): MotionVectorFrame;
    getGlobalDisplacement(): {
        dx: number;
        dy: number;
    };
}
export declare class TrajectoryPredictor {
    private aeroCore;
    predictNextPosition(timeStepSec: number): Vector3D;
}
export declare class StabilityScoreProvider {
    private aeroCore;
    getStabilityScore(): number;
    isStable(threshold?: number): boolean;
}
export declare class MotionReactiveAnimation {
    private aeroCore;
    calculateParticleSwirlIntensity(): number;
    calculateDodgeOffset(): Vector3D;
}
//# sourceMappingURL=LensStudioSDK.d.ts.map