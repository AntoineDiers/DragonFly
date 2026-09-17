#pragma once

#include <cstdint>

namespace config::flight_controller::gpio
{
    enum Pins
    {
        // ACTUATORS
        ESC_PWM = 20,
        LEFT_FLAPERON_PWM = 21,
        RIGHT_FLAPERON_PWM = 22,
        LEFT_RUDDERVATOR_PWM = 26,
        RIGHT_RUDDERVATOR_PWM = 27,

        // RF UART
        RF_UART_TX = 8,
        RF_UART_RX = 9,
        RF_UART_CTS = 6,
        RF_UART_RTS = 7,

        // PC SPI
        PC_SPI_SCK = 2,
        PC_SPI_TX = 3,
        PC_SPI_RX = 4,
        PC_SPI_CSN = 5,

        // GPS UART
        GPS_UART_RX = 1,
        GPS_UART_TX = 0,

        // I2C BUS 0
        I2C_0_SDA = 16,
        I2C_0_SCL = 17,

        // I2C BUS 1
        I2C_1_SDA = 14,
        I2C_1_SCL = 15,
        
        // IMUs
        PRIMARY_IMU_INT = 11,
        PRIMARY_IMU_RST = 10,
        BACKUP_IMU_INT = 18,
        BACKUP_IMU_RST = 19,

        // AIRSPEED SENSOR
        AIRSPEED_SENSOR_INT = 12,

        // PRESSURE SENSOR
        PRESSURE_SENSOR_INT = 13,

        // SYSTEM LED
        SYSTEM_LED = 25,
    };
};