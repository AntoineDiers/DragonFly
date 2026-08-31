#pragma once

#include <rpi_pico_utils/utils/Periodic.h>
#include <ads1115_driver/Ads1115Driver.h>
#include <rpi_pico_utils/timer/Timer.h>

#include <dragonfly_msgs/msg/BatteryStats.h>

#include "BatteryLevelKalman.h"

class BatteryStatsHandler
{
public:

    struct Config
    {
        Ads1115Driver::Config ads1115_config;
        Ads1115Driver::Pin voltage_pin;
        Ads1115Driver::Pin current_pin;
        Ads1115Driver::Pin current_after_esc_pin;
        Ads1115Driver::Pin current_after_servos_pin;
        float battery_voltage_divider_ratio;
        float acs770_zero_v;
        float acs770_sensitivity_a_per_v;
        float acs712_zero_v;
        float acs712_sensitivity_a_per_v;
        uint32_t ads1115_data_timeout_ms;
        uint32_t kalman_period_ms;
        BatteryLevelKalman::Config kalman_config;
    };

    BatteryStatsHandler(const Config& config, FlightControllerInputs* fc_inputs,  ClockInterface* clock, IoInterface* ads1115_i2c) : 
        _config(config),
        _fc_inputs(fc_inputs),
        _ads1115_driver(clock, ads1115_i2c, config.ads1115_config),
        _kalman(config.kalman_config, clock),
        _kalman_timer(clock, config.kalman_period_ms)
        {}

    void tick()
    {
        // Read ads1115
        std::optional<Ads1115Driver::Reading> reading = _ads1115_driver.tick();
        if(reading.has_value())
        {
            if(reading->pin == _config.voltage_pin) 
            { 
                _fc_inputs->battery_voltage.update(reading->voltage / _config.battery_voltage_divider_ratio); 
            }
            if(reading->pin == _config.current_pin) 
            { 
                _fc_inputs->battery_current.update((reading->voltage - _config.acs770_zero_v) / _config.acs770_sensitivity_a_per_v);
            }
            if(reading->pin == _config.current_after_esc_pin) 
            { 
                _fc_inputs->current_after_esc.update((reading->voltage - _config.acs712_zero_v) / _config.acs712_sensitivity_a_per_v);
            }
            if(reading->pin == _config.current_after_servos_pin) 
            { 
                _fc_inputs->current_after_servos.update((reading->voltage - _config.acs712_zero_v) / _config.acs712_sensitivity_a_per_v);
            }
        }

        // Update kalman
        if(_kalman_timer.poll())
        {
            std::optional<float> voltage = _fc_inputs->battery_voltage.getLastValue(_config.ads1115_data_timeout_ms);
            std::optional<float> current = _fc_inputs->battery_current.getLastValue(_config.ads1115_data_timeout_ms);

            if(voltage && current)
            {
                BatteryLevelKalman::Input input = 
                {
                    .current = current.value(),
                    .voltage = voltage.value()
                };
                _fc_inputs->battery_level.update(_kalman.tick(input));
            }
        }
    }

private:

    Config _config;

    FlightControllerInputs* _fc_inputs;
    Ads1115Driver _ads1115_driver;
    BatteryLevelKalman _kalman;

    Timer _kalman_timer;
};