#include "OpenLoopLanding.h"
#include <config/flight_controller/control_loop/fallback_modes/Config.h>

using namespace config::flight_controller::control_loop::fallback_modes::open_loop_landing;
using namespace config::flight_controller::control_loop::fallback_modes;

dragonfly_msgs::msg::ActuatorCommands fallback_modes::open_loop_landing::tick(
    const dragonfly_msgs::msg::Altitude &altitude)
{
    float throttle_percent = 0;
    if(altitude.altitude_m > MIN_THROTTLE_ALTITUDE_M)
    {
        throttle_percent = 
            MAX_THROTTLE_PERCENT * 
            (altitude.altitude_m - MIN_THROTTLE_ALTITUDE_M) / 
            (SAFE_ALTITDE_LIMIT_M - MIN_THROTTLE_ALTITUDE_M);
    }

    dragonfly_msgs::msg::ActuatorCommands res;
    res.throttle_percent = throttle_percent;
    res.flaps_deg = FLAPS_DEG;
    res.ailerons_deg = 0;
    res.elevator_deg = ELEVATOR_DEG;
    res.rudder_deg = 0;
    return res;
}