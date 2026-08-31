#include "ClosedLoopLanding.h"
#include <config/flight_controller/control_loop/fallback_modes/Config.h>

using namespace config::flight_controller::control_loop::fallback_modes::closed_loop_landing;
using namespace config::flight_controller::control_loop::fallback_modes;

dragonfly_msgs::msg::ActuatorCommands fallback_modes::closed_loop_landing::tick(
    const FlightControllerInputs::Attitude &attitude, 
    const float &altitude, 
    Pids &pids)
{
    float throttle_percent = 0;
    if(altitude > ::MIN_THROTTLE_ALTITUDE_M)
    {
        throttle_percent = 
            MAX_THROTTLE_PERCENT * 
            (altitude - MIN_THROTTLE_ALTITUDE_M) / 
            (SAFE_ALTITDE_LIMIT_M - MIN_THROTTLE_ALTITUDE_M);
    }

    dragonfly_msgs::msg::ActuatorCommands res;
    res.throttle_percent = throttle_percent;
    res.flaps_deg = FLAPS_DEG;
    res.ailerons_deg = pids.roll.tick(0, attitude.roll_deg);
    res.elevator_deg = pids.pitch.tick(PITCH_SETPOINT_DEG, attitude.pitch_deg);
    res.rudder_deg = 0;
    return res;
}