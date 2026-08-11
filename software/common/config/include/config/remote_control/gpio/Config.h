#pragma once

#include <cstdint>
#include <rpi_pico_utils/adc/AdcImplementation.h>

namespace config::remote_control::gpio
{
    enum Pins
    {
        // ADS 1115
        ADS_1115_SCL = 0,
        ADS_1115_SDA = 1,

        // RF UART
        RF_UART_RX,
        RF_UART_TX,
        RF_UART_RTS,
        RF_UART_CTS,

        // ADCs
        THROTTLE_ADC = 26,
        LEFT_RIGHT_ADC = 27,
        DOWN_UP_ADC = 28,

        // SWITCHES
        ARM_SWITCH,
        AUTO_SWITCH,

        // LEDS
        LINK_LED_R,
        LINK_LED_G,
        STATUS_LED_R,
        STATUS_LED_G,
        PICO_LED = 25,
    };
};