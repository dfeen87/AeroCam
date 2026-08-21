import { UnifiedMotionState } from "./types";
export declare class AeroCoreService {
    private static instance;
    private eventBus;
    private currentState;
    private lastVelocityMagnitude;
    private constructor();
    static getInstance(): AeroCoreService;
    private createInitialState;
    ingestMotionState(partialState: Partial<UnifiedMotionState>): void;
    getState(): UnifiedMotionState;
    private evaluateAndPublishEvents;
}
//# sourceMappingURL=AeroCoreService.d.ts.map