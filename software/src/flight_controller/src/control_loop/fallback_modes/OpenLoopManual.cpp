#include "OpenLoopManual.h"
#include <config/flight_controller/control_loop/fallback_modes/Config.h>
#include <config/flight_controller/actuators/limits/Config.h>
#include "../ActuatorCommands.h"

#include <dragonfly_msgs/msgs/remote_control/Inputs.h>

using namespace config::flight_controller::actuators::limits;

ActuatorCommands fallback_modes::open_loop_manual::tick(
    const dragonfly_msgs::msgs::remote_control::Inputs &remote_control_inputs)
{
    ActuatorCommands res;
    res.throttle_percent = remote_control_inputs.throttle_percent.get();
    res.flaps_deg = remote_control_inputs.flaps_deg.get();
    res.ailerons_deg = - remote_control_inputs.left_right.get() * MAX_AILERON_ANGLE_DEG;;
    res.elevator_deg = remote_control_inputs.down_up.get() * MAX_AILERON_ANGLE_DEG;
    res.rudder_deg = - remote_control_inputs.left_right.get() * MAX_RUDDER_ANGLE_DEG;
    return res;
}   