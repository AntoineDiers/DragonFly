#pragma once

#include <cstdint>
#include <rpi_pico_utils/adc/AdcImplementation.h>

namespace config::remote_control::gpio
{
    enum Pins
    {
        // ADS 1115
        ADS_1115_SDA = 20,
        ADS_1115_SCL = 21,

        // RF UART
        RF_UART_TX = 0,
        RF_UART_RX = 1,
        RF_UART_CTS = 2,
        RF_UART_RTS = 3,

        // ADC
        DOWN_UP_ADC = 26,
        LEFT_RIGHT_ADC = 27,
        THROTTLE_ADC = 28,

        // SWITCHES
        ARM_SWITCH = 16,
        AUTO_SWITCH = 17,

        // LEDS
        LINK_LED_R = 4,
        LINK_LED_G = 5,
        STATUS_LED_R = 6,
        STATUS_LED_G = 7,
        
        PICO_LED = 25,
    };
};