#pragma once

#include "../ActuatorCommands.h"

#include "../Pids.h"
#include "../../inputs/FlightControllerInputs.h"

namespace fallback_modes::closed_loop_landing
{
    ActuatorCommands tick(
        const FlightControllerInputs::Attitude& attitude,
        const float& altitude,
        Pids& pids);
}