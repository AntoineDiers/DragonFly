#pragma once

#include <dragonfly_msgs/msgs.h>
#include "../Pids.h"

#include "../../inputs/FlightControllerInputs.h"

namespace control_modes::manual
{
    dragonfly_msgs::msg::ActuatorCommands tick(
        const dragonfly_msgs::msg::remote_control::Inputs& remote_control_inputs,
        const FlightControllerInputs::Attitude& attitude,
        Pids& pids);
}