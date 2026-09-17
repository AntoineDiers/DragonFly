#pragma once

#include "../ActuatorCommands.h"

#include "../Pids.h"

#include <dragonfly_msgs/msgs/remote_control/Inputs.h>

namespace fallback_modes::open_loop_manual
{
    ActuatorCommands tick(
        const dragonfly_msgs::msgs::remote_control::Inputs& remote_control_inputs); 
}