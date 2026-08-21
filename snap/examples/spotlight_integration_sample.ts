// Spotlight Integration Sample: Automated Motion Quality Score & Ingestion Filter
import {
  SpotlightMotionRankingAPI,
  AeroCoreService
} from "../ts/src/index";

function processSpotlightVideoUpload(clipId: string): void {
  console.log(`Processing Spotlight Video Upload for Clip: ${clipId}...`);

  const rankingAPI = new SpotlightMotionRankingAPI();
  const result = rankingAPI.evaluateClipForSpotlight(clipId);

  console.log("--------------------------------------------------");
  console.log(`Spotlight Motion Quality Metrics for Clip ${result.clipId}:`);
  console.log(`- Temporal Coherence Score : ${(result.metrics.temporalCoherenceScore * 100).toFixed(1)}%`);
  console.log(`- Scene Stability Score    : ${(result.metrics.sceneStabilityScore * 100).toFixed(1)}%`);
  console.log(`- Overall Quality Index    : ${(result.metrics.overallQualityIndex * 100).toFixed(1)}%`);
  console.log(`- Recommended for Feed    : ${result.recommendedForFeed ? "YES" : "NO"}`);
  console.log(`- Ranking Score Multiplier : ${result.rankingScoreBoost}x`);
  console.log("--------------------------------------------------");

  if (result.recommendedForFeed) {
    console.log(`✅ Clip ${clipId} promoted to Spotlight Feed with ${result.rankingScoreBoost}x rank boost!`);
  } else {
    console.log(`⚠️ Clip ${clipId} placed in standard queue due to unstable camera shake.`);
  }
}

// Simulate upload evaluation
processSpotlightVideoUpload("snap_clip_2026_001_x9");
