#pragma once

#include <cstdint>
#include <rpi_pico_utils/pwm/PwmImplementation.h>
#include "../gpio/Config.h"

namespace config::remote_control::leds
{
    static constexpr uint32_t LED_BLINK_PERIOD_MS = 250; 

    static constexpr uint32_t ERROR_PING_MS = 200;
    static constexpr uint32_t ERROR_PACKET_LOSS_PERCENT = 30;

    static constexpr uint32_t WARN_PING_MS = 100;
    static constexpr uint32_t WARN_PACKET_LOSS_PERCENT = 10;

    PwmImplementation::Config STATUS_RED_PWM_CONFIG
    {
        .gpio = gpio::STATUS_LED_R,
        .frequency_hz = 1000,
        .tick_frequency_hz = 1000000,
        .min_pulse_width_ticks = 0,
        .max_pulse_width_ticks = 1000
    };

    PwmImplementation::Config STATUS_GREEN_PWM_CONFIG
    {
        .gpio = gpio::STATUS_LED_G,
        .frequency_hz = 1000,
        .tick_frequency_hz = 1000000,
        .min_pulse_width_ticks = 0,
        .max_pulse_width_ticks = 1000
    };

    PwmImplementation::Config LINK_RED_PWM_CONFIG
    {
        .gpio = gpio::LINK_LED_R,
        .frequency_hz = 1000,
        .tick_frequency_hz = 1000000,
        .min_pulse_width_ticks = 0,
        .max_pulse_width_ticks = 1000
    };

    PwmImplementation::Config LINK_GREEN_PWM_CONFIG
    {
        .gpio = gpio::LINK_LED_G,
        .frequency_hz = 1000,
        .tick_frequency_hz = 1000000,
        .min_pulse_width_ticks = 0,
        .max_pulse_width_ticks = 1000
    };

    static constexpr float RED_INTENSITY_PERCENT = 10;
    static constexpr float GREEN_INTENSITY_PERCENT = 2; 
};