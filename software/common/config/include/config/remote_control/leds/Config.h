#pragma once

#include <cstdint>

namespace config::remote_control::leds
{
    static constexpr uint32_t LED_BLINK_PERIOD_MS = 250; 

    static constexpr uint32_t ERROR_PING_MS = 200;
    static constexpr uint32_t ERROR_PACKET_LOSS_PERCENT = 30;

    static constexpr uint32_t WARN_PING_MS = 100;
    static constexpr uint32_t WARN_PACKET_LOSS_PERCENT = 10;
};