// Spectacles Demo: Head-Gesture & AR Alignment Stabilization
import {
  AeroCoreService,
  OnGestureNode,
  TrajectoryPredictor
} from "../ts/src/index";

class SpectaclesMotionDemo {
  private aeroCore = AeroCoreService.getInstance();
  private gestureNode = new OnGestureNode();
  private predictor = new TrajectoryPredictor();

  public init(): void {
    console.log("Starting Spectacles Motion Intelligence Layer...");

    // Listen for hands-free head gestures
    this.gestureNode.bind((gesture, confidence) => {
      console.log(`[Spectacles Gesture] Detected head gesture: ${gesture} (confidence: ${(confidence * 100).toFixed(0)}%)`);
      switch (gesture) {
        case "NOD":
          console.log("-> Executing UI Accept Action");
          break;
        case "SHAKE":
          console.log("-> Executing UI Dismiss Action");
          break;
        case "TILT_LEFT":
        case "TILT_RIGHT":
          console.log("-> Cycling AR display layer");
          break;
      }
    });
  }

  public updateAROverlay(): void {
    const predictedPos = this.predictor.predictNextPosition(0.016); // 60 FPS frame time
    const state = this.aeroCore.getState();

    console.log(`[Spectacles AR Stabilization] Head pose orientation: [w:${state.headPose.orientation.w.toFixed(2)}, x:${state.headPose.orientation.x.toFixed(2)}]`);
    console.log(`[Spectacles AR Alignment Correction] [w:${state.headPose.arAlignmentCorrection.w.toFixed(2)}, x:${state.headPose.arAlignmentCorrection.x.toFixed(2)}]`);
    console.log(`[AR Predictor] Next projected anchor position: x=${predictedPos.x.toFixed(3)}, y=${predictedPos.y.toFixed(3)}`);
  }
}

const demo = new SpectaclesMotionDemo();
demo.init();
demo.updateAROverlay();
