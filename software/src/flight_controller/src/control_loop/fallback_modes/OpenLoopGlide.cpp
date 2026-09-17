#include "OpenLoopGlide.h"
#include <config/flight_controller/control_loop/fallback_modes/Config.h>

using namespace config::flight_controller::control_loop::fallback_modes::open_loop_glide;

ActuatorCommands fallback_modes::open_loop_glide::tick()
{
    ActuatorCommands res;
    res.throttle_percent = THROTTLE_PERCENT;
    res.flaps_deg = FLAPS_DEG;
    res.ailerons_deg = 0;
    res.elevator_deg = ELEVATOR_DEG;
    res.rudder_deg = 0;
    return res;
}