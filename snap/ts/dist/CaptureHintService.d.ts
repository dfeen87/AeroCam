export interface CaptureHint {
    status: "steady" | "moving" | "unstable";
    message: string;
    confidence: number;
}
export declare class CaptureHintService {
    private aeroCore;
    private currentHint;
    constructor();
    getHint(): CaptureHint;
}
//# sourceMappingURL=CaptureHintService.d.ts.map