"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.SpotlightMotionRankingAPI = void 0;
const AeroCoreService_1 = require("./AeroCoreService");
class SpotlightMotionRankingAPI {
    aeroCore = AeroCoreService_1.AeroCoreService.getInstance();
    evaluateClipForSpotlight(clipId) {
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
exports.SpotlightMotionRankingAPI = SpotlightMotionRankingAPI;
//# sourceMappingURL=SpotlightRankingAPI.js.map