#pragma once

#include <rpi_pico_utils/uart/UartImplementation.h>

#include "../gpio/Config.h"

namespace config::remote_control::comms
{
    static constexpr UartImplementation::Config RF_UART_CONFIG =
    {
        .uart = UartId::UART_0,
        .baudrate = 57600,
        .gpio_rx = gpio::RF_UART_RX,
        .gpio_tx = gpio::RF_UART_TX,
        .gpio_rts = gpio::RF_UART_RTS,
        .gpio_cts = gpio::RF_UART_CTS
    };

    I2cImplementation::Config I2C_CONFIG = 
    {
        .i2c_inst = i2c0,
        .scl_gpio = gpio::ADS_1115_SCL,
        .sda_gpio = gpio::ADS_1115_SDA,
        .baudrate = 100000,
        .rw_timeout_ms = 3
    };
}