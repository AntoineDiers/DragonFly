#pragma once

#include <cstdint>

#include "../gpio/Config.h"

namespace config::flight_controller::led
{
    static constexpr uint32_t SYSTEM_LED_PERIOD_MS = 500;
}