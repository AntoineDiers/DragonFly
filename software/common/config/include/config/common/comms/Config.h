#pragma once

#include <cstdint>

namespace config::common::comms
{
    static constexpr uint32_t PING_PERIOD_MS = 100; 
    static constexpr uint32_t PACKET_LOSS_WINDOW_SIZE = 30;
    static constexpr uint32_t PING_TTL_MS = 200;
};