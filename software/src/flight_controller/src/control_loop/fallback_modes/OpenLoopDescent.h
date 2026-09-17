#pragma once

#include "../ActuatorCommands.h"

#include "../Pids.h"

namespace fallback_modes::open_loop_descent
{
    ActuatorCommands tick();
}