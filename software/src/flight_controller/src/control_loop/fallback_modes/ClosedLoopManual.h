#pragma once

#include "../control_modes/Manual.h"
#include "../../inputs/FlightControllerInputs.h"

#include <dragonfly_msgs/msgs/remote_control/Inputs.h>

namespace fallback_modes::closed_loop_manual
{
    ActuatorCommands tick(
        const dragonfly_msgs::msgs::remote_control::Inputs& remote_control_inputs,
        const FlightControllerInputs::Attitude& attitude,
        Pids& pids);
}