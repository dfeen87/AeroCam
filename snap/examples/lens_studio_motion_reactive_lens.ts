// Lens Studio Creator Example: Motion-Reactive Particle Swirl Lens
import {
  OnTiltNode,
  OnSpinNode,
  OnVelocitySpikeNode,
  MotionReactiveAnimation,
  CaptureHintService
} from "../ts/src/index";

class ParticleSwirlLensController {
  private tiltNode = new OnTiltNode();
  private spinNode = new OnSpinNode();
  private spikeNode = new OnVelocitySpikeNode();
  private animationHelper = new MotionReactiveAnimation();
  private hintService = new CaptureHintService();

  public onAwake(): void {
    console.log("Initializing Motion-Reactive Particle Swirl Lens...");

    // Bind tilt events
    this.tiltNode.bind((angle, direction) => {
      console.log(`[Lens Event] Device tilted ${direction} by ${angle.toFixed(2)} rad`);
      // Update AR particle swirl direction in Lens Studio 3D scene
    });

    // Bind spin events
    this.spinNode.bind((angularSpeed, axis) => {
      const swirlIntensity = this.animationHelper.calculateParticleSwirlIntensity();
      console.log(`[Lens Event] Device spinning around ${axis} at ${angularSpeed.toFixed(2)} rad/s | Swirl Intensity: ${swirlIntensity.toFixed(2)}`);
      // Accelerate particle swirl emitter
    });

    // Bind velocity spike events (e.g. sharp gesture or camera burst)
    this.spikeNode.bind((magnitude) => {
      console.log(`[Lens Event] Motion velocity spike detected! Magnitude: ${magnitude.toFixed(2)}`);
      // Trigger burst particle explosion animation
    });
  }

  public onUpdate(): void {
    const hint = this.hintService.getHint();
    if (hint.status !== "steady") {
      // Display UI guidance hint banner on screen
      console.log(`[UI Hint] ${hint.message}`);
    }
  }
}

// Instantiate Lens controller
const lensController = new ParticleSwirlLensController();
lensController.onAwake();
lensController.onUpdate();
