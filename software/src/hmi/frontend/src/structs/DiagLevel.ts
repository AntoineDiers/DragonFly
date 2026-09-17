export enum DiagLevel {
  OK,
  WARN,
  ERR,
  UNKNOWN
}

export namespace DiagLevel {
  export function Default() : DiagLevel {
        return DiagLevel.UNKNOWN;
    }
}