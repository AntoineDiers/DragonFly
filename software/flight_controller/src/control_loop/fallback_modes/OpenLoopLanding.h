#pragma once

#include <dragonfly_msgs/msgs.h>

#include "../Pids.h"

namespace fallback_modes::open_loop_landing
{
    dragonfly_msgs::msg::ActuatorCommands tick(
        const dragonfly_msgs::msg::Altitude& altitude);
}