#pragma once

#include <rpi_pico_utils/uart/UartImplementation.h>
#include <rpi_pico_utils/i2c/I2cImplementation.h>

#include "../gpio/Config.h"

namespace config::flight_controller::comms
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

    static constexpr UartImplementation::Config PAYLOAD_COMPUTER_UART_CONFIG =
    {
        .uart = UartId::UART_1,
        .baudrate = 115200,
        .gpio_rx = gpio::PC_UART_RX,
        .gpio_tx = gpio::PC_UART_TX,
        .gpio_rts = gpio::PC_UART_RTS,
        .gpio_cts = gpio::PC_UART_CTS
    };

    I2cImplementation::Config I2C_0
    {
        .i2c_inst = i2c0,
        .scl_gpio = gpio::I2C_0_SCL,
        .sda_gpio = gpio::I2C_0_SDA,
        .baudrate = 400000,
        .rw_timeout_ms = 10,
    };

    I2cImplementation::Config I2C_1
    {
        .i2c_inst = i2c1,
        .scl_gpio = gpio::I2C_1_SCL,
        .sda_gpio = gpio::I2C_1_SDA,
        .baudrate = 400000,
        .rw_timeout_ms = 10,
    };
}