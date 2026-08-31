#pragma once

#include <dragonfly_msgs/msgs.h>

#include "../Pids.h"

namespace fallback_modes::open_loop_manual
{
    dragonfly_msgs::msg::ActuatorCommands tick(
        const dragonfly_msgs::msg::remote_control::Inputs& remote_control_inputs); 
}