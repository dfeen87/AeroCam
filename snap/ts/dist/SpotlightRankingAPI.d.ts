import { SpotlightMotionMetrics } from "./types";
export interface SpotlightIngestionResponse {
    clipId: string;
    metrics: SpotlightMotionMetrics;
    recommendedForFeed: boolean;
    rankingScoreBoost: number;
}
export declare class SpotlightMotionRankingAPI {
    private aeroCore;
    evaluateClipForSpotlight(clipId: string): SpotlightIngestionResponse;
}
//# sourceMappingURL=SpotlightRankingAPI.d.ts.map