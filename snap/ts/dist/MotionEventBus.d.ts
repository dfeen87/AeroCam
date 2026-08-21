import { MotionEventType, MotionEventPayloadMap, MotionEventListener } from "./types";
export declare class MotionEventBus {
    private static instance;
    private listeners;
    private constructor();
    static getInstance(): MotionEventBus;
    subscribe<T extends MotionEventType>(event: T, listener: MotionEventListener<T>): () => void;
    unsubscribe<T extends MotionEventType>(event: T, listener: MotionEventListener<T>): void;
    publish<T extends MotionEventType>(event: T, payload: MotionEventPayloadMap[T]): void;
    clear(): void;
}
//# sourceMappingURL=MotionEventBus.d.ts.map