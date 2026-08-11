#pragma once

#include <dragonfly_msgs/msgs.h>
#include "../Pids.h"

namespace control_modes::manual
{
    dragonfly_msgs::msg::ActuatorCommands tick(
        const dragonfly_msgs::msg::RemoteControlInputs& remote_control_inputs,
        const dragonfly_msgs::msg::Attitude& attitude,
        Pids& pids);
}