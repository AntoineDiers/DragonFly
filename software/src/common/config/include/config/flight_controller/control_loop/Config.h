#pragma once

#include <rpi_pico_utils/utils/Periodic.h>

namespace config::flight_controller::control_loop
{
    static constexpr uint32_t PERIOD_MS = 20;

    static constexpr uint32_t REMOTE_CONTROL_TIMEOUT_MS = 200;
    static constexpr uint32_t ATTITUDE_TIMEOUT_MS = 200;
    static constexpr uint32_t ALTITUDE_TIMEOUT_MS = 200;
    static constexpr uint32_t BATTERY_LEVEL_TIMEOUT = 10000;
}