#include "ClosedLoopManual.h"
#include <config/flight_controller/control_loop/fallback_modes/Config.h>

dragonfly_msgs::msg::ActuatorCommands fallback_modes::closed_loop_manual::tick(
    const dragonfly_msgs::msg::remote_control::Inputs &remote_control_inputs, 
    const FlightControllerInputs::Attitude &attitude, 
    Pids &pids)
{
    return control_modes::manual::tick(
        remote_control_inputs, 
        attitude,
        pids);
}