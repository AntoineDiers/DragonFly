#pragma once

#include <dragonfly_msgs/msgs.h>

#include "../Pids.h"

namespace fallback_modes::closed_loop_orbit
{
    dragonfly_msgs::msg::ActuatorCommands tick(
        const dragonfly_msgs::msg::Attitude& attitude,
        const dragonfly_msgs::msg::Altitude& altitude,
        Pids& pids);
}