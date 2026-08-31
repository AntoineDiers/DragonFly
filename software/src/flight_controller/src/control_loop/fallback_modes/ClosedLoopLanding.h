#pragma once

#include <dragonfly_msgs/msgs.h>

#include "../Pids.h"
#include "../../inputs/FlightControllerInputs.h"

namespace fallback_modes::closed_loop_landing
{
    dragonfly_msgs::msg::ActuatorCommands tick(
        const FlightControllerInputs::Attitude& attitude,
        const float& altitude,
        Pids& pids);
}