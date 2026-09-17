#pragma once

#include "../hardware/HardwareInterface.h"
#include <ads1115_driver/Ads1115Driver.h>

#include <dragonfly_msgs/msgs/remote_control/Inputs.h>
#include <config/remote_control/inputs/Config.h>
#include <config/remote_control/inputs/ads_1115/Config.h>

#include <algorithm>
#include <optional>

class Inputs
{
public:

    Inputs(const HardwareInterface& hw_interface) : 
        _hw_interface(hw_interface),
        _ads_1115_driver(hw_interface.clock, hw_interface.ads_1115_i2c, config::remote_control::inputs::ads_1115::CONFIG),
        _roll_stab_voltage(config::remote_control::inputs::ads_1115::INPUTS_RATE, hw_interface.clock),
        _pitch_stab_voltage(config::remote_control::inputs::ads_1115::INPUTS_RATE, hw_interface.clock),
        _flaps_voltage(config::remote_control::inputs::ads_1115::INPUTS_RATE, hw_interface.clock) {}

    void tick()
    {
        std::optional<Ads1115Driver::Reading> ads1115_reading = _ads_1115_driver.tick();
        if(ads1115_reading)
        {
            switch (ads1115_reading->pin)
            {
                case config::remote_control::inputs::ads_1115::ROLL_STAB_PIN :  { _roll_stab_voltage.update(ads1115_reading->voltage); break; }
                case config::remote_control::inputs::ads_1115::PITCH_STAB_PIN : { _pitch_stab_voltage.update(ads1115_reading->voltage); break; }
                case config::remote_control::inputs::ads_1115::FLAPS_PIN :      { _flaps_voltage.update(ads1115_reading->voltage); break; }
                default: break;
            }
        }
    }

    dragonfly_msgs::msgs::remote_control::Inputs read(bool& read_error)
    {
        dragonfly_msgs::msgs::remote_control::Inputs res;

        res.arm_enabled = _hw_interface.arm_switch->read();
        res.auto_enabled = _hw_interface.auto_switch->read();
        
        res.down_up =          convertAdcReading(_hw_interface.down_up_stick->readVoltage(), config::remote_control::inputs::DOWN_UP_ADC_CONFIG);
        res.left_right =       convertAdcReading(_hw_interface.left_right_stick->readVoltage(), config::remote_control::inputs::LEFT_RIGHT_ADC_CONFIG);
        res.throttle_percent = convertAdcReading(_hw_interface.throttle_stick->readVoltage(), config::remote_control::inputs::THROTTLE_ADC_CONFIG);

        std::optional<float> flaps_voltage = _flaps_voltage.getLastValue(config::remote_control::inputs::ads_1115::INPUTS_TIMEOUT_MS);
        std::optional<float> roll_stab_voltage = _roll_stab_voltage.getLastValue(config::remote_control::inputs::ads_1115::INPUTS_TIMEOUT_MS);
        std::optional<float> pitch_stab_voltage = _pitch_stab_voltage.getLastValue(config::remote_control::inputs::ads_1115::INPUTS_TIMEOUT_MS);

        read_error = !(flaps_voltage && roll_stab_voltage && pitch_stab_voltage);

        res.flaps_deg = convertAdcReading(flaps_voltage.value_or(0), config::remote_control::inputs::FLAPS_ADC_CONFIG);
        res.roll_stab_level_percent = convertAdcReading(roll_stab_voltage.value_or(0), config::remote_control::inputs::ROLL_STAB_ADC_CONFIG);
        res.pitch_stab_level_percent = convertAdcReading(pitch_stab_voltage.value_or(0), config::remote_control::inputs::PITCH_STAB_ADC_CONFIG);

        return res;
    }

private:

    float convertAdcReading(float voltage, const config::remote_control::inputs::AdcConfig& conf)
    {
        float ratio = std::clamp(
            (voltage - conf.min_voltage) / (conf.max_voltage - conf.min_voltage),
            0.0f,
            1.0f);

        if(conf.inverted) { ratio = 1.0f - ratio; }

        if(conf.center_dead_zone && std::abs(0.5f - ratio) < conf.center_dead_zone.value())
        {
            ratio = 0.5;
        }

        ratio = std::clamp(ratio, 0.0f, 1.0f);

        return conf.min_val + ratio * (conf.max_val - conf.min_val);
    }

    HardwareInterface _hw_interface;
    Ads1115Driver _ads_1115_driver;

    Periodic<float> _roll_stab_voltage;
    Periodic<float> _pitch_stab_voltage;
    Periodic<float> _flaps_voltage;
};