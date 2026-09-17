export interface Actuators
{
    throttle_percent : number;
    left_flaperon_deg : number;
    right_flaperon_deg : number;
    left_ruddervator_deg : number;
    right_ruddervator_deg : number;
}

export namespace Actuators {
    export function Default() : Actuators {
        return {
            throttle_percent : 0,
            left_flaperon_deg : 0,
            right_flaperon_deg : 0,
            left_ruddervator_deg : 0,
            right_ruddervator_deg : 0 
        };
    }
}