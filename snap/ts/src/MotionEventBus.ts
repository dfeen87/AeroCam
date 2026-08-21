import {
  MotionEventType,
  MotionEventPayloadMap,
  MotionEventListener
} from "./types";

export class MotionEventBus {
  private static instance: MotionEventBus;
  private listeners: Map<MotionEventType, Set<Function>> = new Map();

  private constructor() {}

  public static getInstance(): MotionEventBus {
    if (!MotionEventBus.instance) {
      MotionEventBus.instance = new MotionEventBus();
    }
    return MotionEventBus.instance;
  }

  public subscribe<T extends MotionEventType>(
    event: T,
    listener: MotionEventListener<T>
  ): () => void {
    if (!this.listeners.has(event)) {
      this.listeners.set(event, new Set());
    }
    this.listeners.get(event)!.add(listener);

    return () => {
      this.unsubscribe(event, listener);
    };
  }

  public unsubscribe<T extends MotionEventType>(
    event: T,
    listener: MotionEventListener<T>
  ): void {
    const eventListeners = this.listeners.get(event);
    if (eventListeners) {
      eventListeners.delete(listener);
    }
  }

  public publish<T extends MotionEventType>(
    event: T,
    payload: MotionEventPayloadMap[T]
  ): void {
    const eventListeners = this.listeners.get(event);
    if (eventListeners) {
      eventListeners.forEach((listener) => {
        try {
          listener(payload);
        } catch (e) {
          console.error(`Error in MotionEventBus listener for ${event}:`, e);
        }
      });
    }
  }

  public clear(): void {
    this.listeners.clear();
  }
}
