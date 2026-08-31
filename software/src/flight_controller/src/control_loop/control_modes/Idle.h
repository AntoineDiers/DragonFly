#pragma once

#include <dragonfly_msgs/msgs.h>

namespace control_modes::idle
{
    dragonfly_msgs::msg::ActuatorCommands tick();
}