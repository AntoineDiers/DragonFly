#pragma once

#include "PwmInterface.h"

#include <algorithm>

#include <hardware/pwm.h>
#include <hardware/clocks.h>
#include <hardware/gpio.h>

class PwmImplementation : public PwmInterface
{
public: 

    struct Config
    {
        uint8_t gpio;
        uint32_t frequency_hz;
        uint32_t tick_frequency_hz;
        uint32_t min_pulse_width_ticks;
        uint32_t max_pulse_width_ticks;
    };

    PwmImplementation(const Config& conf) : 
        _conf(conf)
    {
        gpio_set_function(_conf.gpio, GPIO_FUNC_PWM);
        uint32_t slice = pwm_gpio_to_slice_num(_conf.gpio);
        pwm_config config = pwm_get_default_config();

        // Change the PWM tick frequency according to config
        float clkdiv = (float)clock_get_hz(clk_sys) / _conf.tick_frequency_hz;
        pwm_config_set_clkdiv(&config, clkdiv);
        
        // Set the wrap value so that the PWM frequency matches config
        uint32_t wrap = _conf.tick_frequency_hz / _conf.frequency_hz - 1;
        pwm_config_set_wrap(&config, wrap);

        pwm_init(slice, &config, true);

        // Set command to min
        pwm_set_gpio_level(_conf.gpio, _conf.min_pulse_width_ticks);
    }

    virtual void setCommandPercent(float cmd_percent) override
    {
        cmd_percent = std::clamp(cmd_percent, 0.0f, 100.0f);

        const uint16_t level = _conf.min_pulse_width_ticks + cmd_percent * 
            (_conf.max_pulse_width_ticks - _conf.min_pulse_width_ticks) / 100.0f;

        pwm_set_gpio_level(_conf.gpio, level);
    }

private:

    Config _conf;
};