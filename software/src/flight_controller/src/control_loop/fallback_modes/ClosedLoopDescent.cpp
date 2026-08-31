#include "ClosedLoopDescent.h"
#include <config/flight_controller/control_loop/fallback_modes/Config.h>

using namespace config::flight_controller::control_loop::fallback_modes::closed_loop_descent;

dragonfly_msgs::msg::ActuatorCommands fallback_modes::closed_loop_descent::tick(
    const FlightControllerInputs::Attitude &attitude, 
    Pids &pids)
{
    dragonfly_msgs::msg::ActuatorCommands res;
    res.throttle_percent = THROTTLE_PERCENT;
    res.flaps_deg = FLAPS_DEG;
    res.ailerons_deg = pids.roll.tick(0, attitude.roll_deg);
    res.elevator_deg = pids.pitch.tick(PITCH_SETPOINT_DEG, attitude.pitch_deg);
    res.rudder_deg = 0;
    return res;
}
