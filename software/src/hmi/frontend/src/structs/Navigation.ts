export interface Navigation
{
    roll_deg : number;
    pitch_deg : number;
    airspeed_km_h : number;
    altitude_m : number;
}

export namespace Navigation {
    export function Default() : Navigation {
        return {
            roll_deg : 0,
            pitch_deg : 0,
            airspeed_km_h : 0,
            altitude_m : 0
        };
    }
}