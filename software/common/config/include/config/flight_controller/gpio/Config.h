#pragma once

#include <cstdint>

namespace config::flight_controller::gpio
{
    enum Pins
    {
        // ACTUATORS
        ESC_PWM = 0,
        LEFT_FLAPERON_PWM,
        RIGHT_FLAPERON_PWM,
        ELEVATOR_PWM,
        RUDDER_PWM,

        // RF UART
        RF_UART_RX,
        RF_UART_TX,
        RF_UART_CTS,
        RF_UART_RTS,

        // PC UART
        PC_UART_RX,
        PC_UART_TX,
        PC_UART_CTS,
        PC_UART_RTS,

        // IMU
        IMU_INT_GPIO,
        IMU_RST_GPIO,
        IMU_SCL_GPIO,
        IMU_SDA_GPIO
    };
};