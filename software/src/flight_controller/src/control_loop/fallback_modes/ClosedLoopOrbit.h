#pragma once

#include <dragonfly_msgs/msgs.h>

#include "../Pids.h"
#include "../../inputs/FlightControllerInputs.h"

namespace fallback_modes::closed_loop_orbit
{
    dragonfly_msgs::msg::ActuatorCommands tick(
        const FlightControllerInputs::Attitude& attitude,
        const float& altitude,
        Pids& pids);
}