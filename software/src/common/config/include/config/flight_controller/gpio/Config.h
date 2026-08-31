#pragma once

#include <cstdint>

namespace config::flight_controller::gpio
{
    enum Pins
    {
        // ACTUATORS
        ESC_PWM = 22,
        LEFT_FLAPERON_PWM = 12,
        RIGHT_FLAPERON_PWM = 13,
        ELEVATOR_PWM = 14,
        RUDDER_PWM = 15,

        // RF UART
        RF_UART_TX = 16,
        RF_UART_RX = 17,
        RF_UART_CTS = 18,
        RF_UART_RTS = 19,

        // PC UART
        PC_UART_RX = 8,
        PC_UART_TX = 9,
        PC_UART_CTS = 10,
        PC_UART_RTS = 11,

        // I2C BUS 0
        I2C_0_SDA = 0,
        I2C_0_SCL = 1,

        // I2C BUS 1
        I2C_1_SDA = 0,
        I2C_1_SCL = 1,
        
        // IMUs
        PRIMARY_IMU_INT = 3,
        PRIMARY_IMU_RST = 4,
        BACKUP_IMU_INT = 3,
        BACKUP_IMU_RST = 4,

        // SYSTEM LED
        SYSTEM_LED = 25,
    };
};