import { AeroCoreService } from "./AeroCoreService";

export interface CaptureHint {
  status: "steady" | "moving" | "unstable";
  message: string;
  confidence: number;
}

export class CaptureHintService {
  private aeroCore: AeroCoreService;
  private currentHint: CaptureHint = {
    status: "steady",
    message: "Camera steady",
    confidence: 1.0
  };

  constructor() {
    this.aeroCore = AeroCoreService.getInstance();
  }

  public getHint(): CaptureHint {
    const state = this.aeroCore.getState();
    const stability = state.stabilityScore;
    const motion = state.motionFrame;

    if (stability > 0.8) {
      this.currentHint = {
        status: "steady",
        message: "Great lighting & camera stability!",
        confidence: stability
      };
    } else if (stability > 0.5) {
      if (motion.globalDx < -0.04) {
        this.currentHint = {
          status: "moving",
          message: "Pan left slowly",
          confidence: 0.75
        };
      } else if (motion.globalDx > 0.04) {
        this.currentHint = {
          status: "moving",
          message: "Pan right slowly",
          confidence: 0.75
        };
      } else {
        this.currentHint = {
          status: "moving",
          message: "Keep motion smooth",
          confidence: 0.70
        };
      }
    } else {
      this.currentHint = {
        status: "unstable",
        message: "Hold device steady",
        confidence: 1.0 - stability
      };
    }

    return this.currentHint;
  }
}
