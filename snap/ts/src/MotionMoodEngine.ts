import { AeroCoreService } from "./AeroCoreService";

export interface ColorLUTProfile {
  name: string;
  preset: "CYBERPUNK_NEON" | "CINEMATIC_WARM" | "COOL_ACTION" | "NEUTRAL";
  saturation: number;
  contrast: number;
  colorGradeMatrix: number[]; // 4x4 matrix
}

export interface MotionMoodOutput {
  activeProfile: ColorLUTProfile;
  creativePromptSuggestion: string;
}

export class MotionMoodEngine {
  private aeroCore = AeroCoreService.getInstance();

  public getCurrentMood(): MotionMoodOutput {
    const state = this.aeroCore.getState();
    const velMag = Math.sqrt(
      state.motionFrame.globalDx * state.motionFrame.globalDx +
      state.motionFrame.globalDy * state.motionFrame.globalDy
    );
    const spinRate = Math.abs(state.headPose.angularVelocity.y);

    if (velMag > 0.08 || spinRate > 2.0) {
      return {
        activeProfile: {
          name: "Neon Cyberpunk",
          preset: "CYBERPUNK_NEON",
          saturation: 1.4,
          contrast: 1.3,
          colorGradeMatrix: [
            1.2, 0.0, 0.2, 0.0,
            0.0, 1.0, 0.1, 0.0,
            0.3, 0.0, 1.4, 0.0,
            0.0, 0.0, 0.0, 1.0
          ]
        },
        creativePromptSuggestion: "High-octane action trail with glowing neon particle swirls and futuristic motion blur."
      };
    } else if (velMag > 0.03) {
      return {
        activeProfile: {
          name: "Cool Action",
          preset: "COOL_ACTION",
          saturation: 1.1,
          contrast: 1.2,
          colorGradeMatrix: [
            0.9, 0.0, 0.1, 0.0,
            0.0, 1.0, 0.1, 0.0,
            0.1, 0.1, 1.2, 0.0,
            0.0, 0.0, 0.0, 1.0
          ]
        },
        creativePromptSuggestion: "Dynamic gliding camera aura with cool blue speed vectors and atmospheric light rays."
      };
    } else if (state.stabilityScore > 0.85) {
      return {
        activeProfile: {
          name: "Cinematic Warm",
          preset: "CINEMATIC_WARM",
          saturation: 1.0,
          contrast: 1.1,
          colorGradeMatrix: [
            1.1, 0.1, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 0.9, 0.0,
            0.0, 0.0, 0.0, 1.0
          ]
        },
        creativePromptSuggestion: "Ultra-steady cinematic portrait with golden hour lighting and subtle bokeh swirl."
      };
    }

    return {
      activeProfile: {
        name: "Neutral",
        preset: "NEUTRAL",
        saturation: 1.0,
        contrast: 1.0,
        colorGradeMatrix: [
          1.0, 0.0, 0.0, 0.0,
          0.0, 1.0, 0.0, 0.0,
          0.0, 0.0, 1.0, 0.0,
          0.0, 0.0, 0.0, 1.0
        ]
      },
      creativePromptSuggestion: "Natural aesthetic ambient Lens with soft depth of field."
    };
  }
}
