#include "OpenLoopManual.h"
#include <config/flight_controller/control_loop/fallback_modes/Config.h>
#include <config/flight_controller/actuators/limits/Config.h>
#include "../Utils.h"

using namespace config::flight_controller::actuators::limits;

dragonfly_msgs::msg::ActuatorCommands fallback_modes::open_loop_manual::tick(
    const dragonfly_msgs::msg::remote_control::Inputs &remote_control_inputs)
{
    dragonfly_msgs::msg::ActuatorCommands res;
    res.throttle_percent = 100 * rcInputToFloat(remote_control_inputs.throttle);
    res.flaps_deg = config::flight_controller::actuators::limits::MAX_FLAP_ANGLE_DEG * rcInputToFloat(remote_control_inputs.flaps);
    res.ailerons_deg = - centeredRcInputToFloat(remote_control_inputs.left_right) * MAX_AILERON_ANGLE_DEG;;
    res.elevator_deg = centeredRcInputToFloat(remote_control_inputs.down_up) * MAX_AILERON_ANGLE_DEG;
    res.rudder_deg = - centeredRcInputToFloat(remote_control_inputs.left_right) * MAX_RUDDER_ANGLE_DEG;
    return res;
}   