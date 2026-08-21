import { HeadGesture } from "./types";
export declare class OnTiltNode {
    private bus;
    bind(callback: (angle: number, direction: "left" | "right" | "up" | "down") => void): () => void;
}
export declare class OnSpinNode {
    private bus;
    bind(callback: (angularSpeed: number, axis: "yaw" | "pitch" | "roll") => void): () => void;
}
export declare class OnVelocitySpikeNode {
    private bus;
    bind(callback: (magnitude: number) => void): () => void;
}
export declare class OnTrajectoryCurveNode {
    private bus;
    bind(callback: (curvatureRadius: number) => void): () => void;
}
export declare class OnGestureNode {
    private bus;
    bind(callback: (gesture: HeadGesture, confidence: number) => void): () => void;
}
//# sourceMappingURL=LensStudioNodes.d.ts.map