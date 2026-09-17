#pragma once

#include "../ActuatorCommands.h"
#include "../Pids.h"

#include "../../inputs/FlightControllerInputs.h"

namespace control_modes::manual
{
    ActuatorCommands tick(
        const dragonfly_msgs::msgs::remote_control::Inputs& remote_control_inputs,
        const FlightControllerInputs::Attitude& attitude,
        Pids& pids);
}