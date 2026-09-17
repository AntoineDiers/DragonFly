import { DiagLevel } from './DiagLevel'

export { DiagLevel }

export interface InputDiagsOverview
{
    remote_control : DiagLevel;
    airspeed : DiagLevel;
    attitude : DiagLevel;
    altitude : DiagLevel;
    gps : DiagLevel;
    battery_level : DiagLevel;
}

export namespace InputDiagsOverview {
    export function Default() : InputDiagsOverview {
        return {
            remote_control : DiagLevel.Default(),
            airspeed : DiagLevel.Default(),
            attitude : DiagLevel.Default(),
            altitude : DiagLevel.Default(),
            gps : DiagLevel.Default(),
            battery_level : DiagLevel.Default()
        };
    }
}