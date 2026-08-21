"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.MotionEventBus = void 0;
class MotionEventBus {
    static instance;
    listeners = new Map();
    constructor() { }
    static getInstance() {
        if (!MotionEventBus.instance) {
            MotionEventBus.instance = new MotionEventBus();
        }
        return MotionEventBus.instance;
    }
    subscribe(event, listener) {
        if (!this.listeners.has(event)) {
            this.listeners.set(event, new Set());
        }
        this.listeners.get(event).add(listener);
        return () => {
            this.unsubscribe(event, listener);
        };
    }
    unsubscribe(event, listener) {
        const eventListeners = this.listeners.get(event);
        if (eventListeners) {
            eventListeners.delete(listener);
        }
    }
    publish(event, payload) {
        const eventListeners = this.listeners.get(event);
        if (eventListeners) {
            eventListeners.forEach((listener) => {
                try {
                    listener(payload);
                }
                catch (e) {
                    console.error(`Error in MotionEventBus listener for ${event}:`, e);
                }
            });
        }
    }
    clear() {
        this.listeners.clear();
    }
}
exports.MotionEventBus = MotionEventBus;
//# sourceMappingURL=MotionEventBus.js.map