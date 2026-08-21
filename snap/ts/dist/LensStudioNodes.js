"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.OnGestureNode = exports.OnTrajectoryCurveNode = exports.OnVelocitySpikeNode = exports.OnSpinNode = exports.OnTiltNode = void 0;
const MotionEventBus_1 = require("./MotionEventBus");
class OnTiltNode {
    bus = MotionEventBus_1.MotionEventBus.getInstance();
    bind(callback) {
        return this.bus.subscribe("tilt", (payload) => {
            callback(payload.angle, payload.direction);
        });
    }
}
exports.OnTiltNode = OnTiltNode;
class OnSpinNode {
    bus = MotionEventBus_1.MotionEventBus.getInstance();
    bind(callback) {
        return this.bus.subscribe("spin", (payload) => {
            callback(payload.angularSpeed, payload.axis);
        });
    }
}
exports.OnSpinNode = OnSpinNode;
class OnVelocitySpikeNode {
    bus = MotionEventBus_1.MotionEventBus.getInstance();
    bind(callback) {
        return this.bus.subscribe("velocity_spike", (payload) => {
            callback(payload.magnitude);
        });
    }
}
exports.OnVelocitySpikeNode = OnVelocitySpikeNode;
class OnTrajectoryCurveNode {
    bus = MotionEventBus_1.MotionEventBus.getInstance();
    bind(callback) {
        return this.bus.subscribe("trajectory_curve", (payload) => {
            callback(payload.curvatureRadius);
        });
    }
}
exports.OnTrajectoryCurveNode = OnTrajectoryCurveNode;
class OnGestureNode {
    bus = MotionEventBus_1.MotionEventBus.getInstance();
    bind(callback) {
        return this.bus.subscribe("gesture", (payload) => {
            callback(payload.gesture, payload.confidence);
        });
    }
}
exports.OnGestureNode = OnGestureNode;
//# sourceMappingURL=LensStudioNodes.js.map