#pragma once

#include "../control_modes/Manual.h"

namespace fallback_modes::closed_loop_manual
{
    dragonfly_msgs::msg::ActuatorCommands tick(
        const dragonfly_msgs::msg::RemoteControlInputs& remote_control_inputs,
        const dragonfly_msgs::msg::Attitude& attitude,
        Pids& pids);
}