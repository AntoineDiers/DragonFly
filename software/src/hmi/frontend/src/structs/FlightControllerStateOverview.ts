import {ControlModes} from "./ControlModes"
import {Navigation} from "./Navigation"
import {Actuators} from "./Actuators"
import {InputDiagsOverview, DiagLevel} from "./InputDiagsOverview"

export {ControlModes};
export {Navigation};
export {Actuators};
export {InputDiagsOverview, DiagLevel};

export interface FlightControllerStateOverview
{
    control_mode : ControlModes;
    actuators : Actuators;
    navigation : Navigation;
    input_diags : InputDiagsOverview;
    battery_level_percent : number;
}

export namespace FlightControllerStateOverview {
    export function Default() : FlightControllerStateOverview {
        return {
            control_mode : ControlModes.MANUAL,
            actuators : { 
                throttle_percent : 45,
                left_flaperon_deg : 0,
                right_flaperon_deg : 0,
                left_ruddervator_deg : 0,
                right_ruddervator_deg : 0 
            },
            navigation : {
                roll_deg : -20,
                pitch_deg : 1,
                airspeed_km_h : 62,
                altitude_m : 254,
            },
            input_diags : {
                remote_control : DiagLevel.OK,
                airspeed : DiagLevel.OK,
                attitude : DiagLevel.OK,
                altitude : DiagLevel.OK,
                gps : DiagLevel.OK,
                battery_level : DiagLevel.OK,
            },
            battery_level_percent : 72 
        };
    }
}