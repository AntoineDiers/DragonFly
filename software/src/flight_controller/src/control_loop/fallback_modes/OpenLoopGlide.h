#pragma once

#include <dragonfly_msgs/msgs.h>

#include "../Pids.h"

namespace fallback_modes::open_loop_glide
{
    dragonfly_msgs::msg::ActuatorCommands tick();
}