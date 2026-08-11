#pragma once

#include <dragonfly_msgs/msgs.h>

#include "../Pids.h"

namespace fallback_modes::closed_loop_descent
{
    dragonfly_msgs::msg::ActuatorCommands tick(
        const dragonfly_msgs::msg::Attitude& attitude,
        Pids& pids);
}