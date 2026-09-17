#pragma once

#include <rpi_pico_utils/pwm/PwmImplementation.h>

#include "../gpio/Config.h"

namespace config::flight_controller::actuators
{
    namespace esc
    {
        static constexpr float DEAD_ZONE_PERCENT = 1;
        static constexpr float MIN_THROTTLE_PERCENT = 20;
    }
    
    static constexpr PwmImplementation::Config ESC_PWM = 
    {
        .gpio = gpio::ESC_PWM,
        .frequency_hz = 50,
        .tick_frequency_hz = 1000000,
        .min_pulse_width_ticks = 1000,
        .max_pulse_width_ticks = 2000
    };

    static constexpr PwmImplementation::Config LEFT_FLAPERON_PWM = 
    {
        .gpio = gpio::LEFT_FLAPERON_PWM,
        .frequency_hz = 50,
        .tick_frequency_hz = 1000000,
        .min_pulse_width_ticks = 1000,
        .max_pulse_width_ticks = 2000
    };

    static constexpr PwmImplementation::Config RIGHT_FLAPERON_PWM = 
    {
        .gpio = gpio::RIGHT_FLAPERON_PWM,
        .frequency_hz = 50,
        .tick_frequency_hz = 1000000,
        .min_pulse_width_ticks = 1000,
        .max_pulse_width_ticks = 2000
    };

    static constexpr PwmImplementation::Config LEFT_RUDDERVATOR_PWM = 
    {
        .gpio = gpio::LEFT_RUDDERVATOR_PWM,
        .frequency_hz = 50,
        .tick_frequency_hz = 1000000,
        .min_pulse_width_ticks = 1000,
        .max_pulse_width_ticks = 2000
    };

    static constexpr PwmImplementation::Config RIGHT_RUDDERVATOR_PWM = 
    {
        .gpio = gpio::RIGHT_RUDDERVATOR_PWM,
        .frequency_hz = 50,
        .tick_frequency_hz = 1000000,
        .min_pulse_width_ticks = 1000,
        .max_pulse_width_ticks = 2000
    };
}