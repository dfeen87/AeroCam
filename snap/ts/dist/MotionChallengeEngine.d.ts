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
    score: number;
    elapsedTimeSec: number;
    rewardUnlocked: boolean;
}
export declare class MotionChallengeEngine {
    private aeroCore;
    private activeChallenge;
    private startTimeMs;
    private accumulatedSteadyTimeMs;
    private accumulatedSpinRad;
    startChallenge(challenge: ChallengeDefinition): void;
    update(dtSec: number): ChallengeProgress | null;
}
//# sourceMappingURL=MotionChallengeEngine.d.ts.map