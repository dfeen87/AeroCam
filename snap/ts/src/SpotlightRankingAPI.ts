import { AeroCoreService } from "./AeroCoreService";
import { SpotlightMotionMetrics } from "./types";

export interface SpotlightIngestionResponse {
  clipId: string;
  metrics: SpotlightMotionMetrics;
  recommendedForFeed: boolean;
  rankingScoreBoost: number; // Multiplier e.g. 1.0 - 1.5x
}

export class SpotlightMotionRankingAPI {
  private aeroCore = AeroCoreService.getInstance();

  public evaluateClipForSpotlight(clipId: string): SpotlightIngestionResponse {
    const metrics = this.aeroCore.getState().spotlightMetrics;
    const boost = 1.0 + (metrics.overallQualityIndex * 0.5);

    return {
      clipId,
      metrics,
      recommendedForFeed: metrics.passesSpotlightThreshold,
      rankingScoreBoost: parseFloat(boost.toFixed(2))
    };
  }
}
