#pragma once

#include <rpi_pico_utils/uart/UartImplementation.h>

#include "../gpio/Config.h"

namespace config::flight_controller::comms
{
    static constexpr UartImplementation::Config RF_UART_CONFIG =
    {
        .GPIO_RX = gpio::RF_UART_RX,
        .GPIO_TX = gpio::RF_UART_TX,
        .GPIO_RTS = gpio::RF_UART_RTS,
        .GPIO_CTS = gpio::RF_UART_CTS
    };

    static constexpr UartImplementation::Config PAYLOAD_COMPUTER_UART_CONFIG =
    {
        .GPIO_RX = gpio::PC_UART_RX,
        .GPIO_TX = gpio::PC_UART_TX,
        .GPIO_RTS = gpio::PC_UART_RTS,
        .GPIO_CTS = gpio::PC_UART_CTS
    };
}