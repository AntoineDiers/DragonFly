#pragma once

#include <cstdint>

namespace config::common::comms
{
    static constexpr uint32_t PING_PERIOD_MS = 500; 
    static constexpr uint32_t PACKET_LOSS_WINDOW_SIZE = 10;
    static constexpr uint32_t PING_TTL_MS = 300;

    static constexpr uint32_t UART_BUFFER_SIZE = 256;
};