export interface ColorLUTProfile {
    name: string;
    preset: "CYBERPUNK_NEON" | "CINEMATIC_WARM" | "COOL_ACTION" | "NEUTRAL";
    saturation: number;
    contrast: number;
    colorGradeMatrix: number[];
}
export interface MotionMoodOutput {
    activeProfile: ColorLUTProfile;
    creativePromptSuggestion: string;
}
export declare class MotionMoodEngine {
    private aeroCore;
    getCurrentMood(): MotionMoodOutput;
}
//# sourceMappingURL=MotionMoodEngine.d.ts.map