#include "OpenLoopManual.h"
#include <config/flight_controller/control_loop/fallback_modes/Config.h>
#include <config/flight_controller/actuators/limits/Config.h>

using namespace config::flight_controller::actuators::limits;

dragonfly_msgs::msg::ActuatorCommands fallback_modes::open_loop_manual::tick(
    const dragonfly_msgs::msg::RemoteControlInputs &remote_control_inputs)
{
    dragonfly_msgs::msg::ActuatorCommands res;
    res.throttle_percent = remote_control_inputs.throttle_percent;
    res.flaps_deg = remote_control_inputs.flaps_angle_deg;
    res.ailerons_deg = - remote_control_inputs.left_right * MAX_AILERON_ANGLE_DEG;;
    res.elevator_deg = remote_control_inputs.down_up * MAX_AILERON_ANGLE_DEG;
    res.rudder_deg = -remote_control_inputs.left_right * MAX_RUDDER_ANGLE_DEG;
    return res;
}   