export enum ControlModes {
  IDLE,
  MANUAL,
  AUTO,
  UNKNOWN
}

export namespace ControlModes {
    export function Default() : ControlModes {
        return ControlModes.UNKNOWN;
    }
}