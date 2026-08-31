#pragma once

#include "../control_modes/Manual.h"
#include "../../inputs/FlightControllerInputs.h"

namespace fallback_modes::closed_loop_manual
{
    dragonfly_msgs::msg::ActuatorCommands tick(
        const dragonfly_msgs::msg::remote_control::Inputs& remote_control_inputs,
        const FlightControllerInputs::Attitude& attitude,
        Pids& pids);
}