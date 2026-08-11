#pragma once

#include "../hardware/HardwareInterface.h"
#include "ads_1115_driver/Ads1115Driver.h"

#include <dragonfly_msgs/msg/remote_control/Inputs.h>
#include <dragonfly_msgs/msg/remote_control/InputsDiagnostics.h>
#include <config/remote_control/inputs/Config.h>

#include <algorithm>
#include <optional>

class Inputs
{
public:

    Inputs(const HardwareInterface& hw_interface) : 
        _hw_interface(hw_interface),
        _ads_1115_driver(hw_interface.clock, hw_interface.ads_1115_i2c) {}

    void tick()
    {
        _ads_1115_driver.tick();
    }

    dragonfly_msgs::msg::remote_control::Inputs read(bool& read_error)
    {
        dragonfly_msgs::msg::remote_control::Inputs res;

        res.is_armed = _hw_interface.arm_switch->read();
        res.auto_mode = _hw_interface.auto_switch->read();
        
        res.down_up = convertAdcReading(_hw_interface.down_up_stick->readVoltage(), config::remote_control::inputs::DOWN_UP_ADC_CONFIG);
        res.left_right = convertAdcReading(_hw_interface.left_right_stick->readVoltage(), config::remote_control::inputs::LEFT_RIGHT_ADC_CONFIG);
        res.throttle_percent = convertAdcReading(_hw_interface.throttle_stick->readVoltage(), config::remote_control::inputs::THROTTLE_ADC_CONFIG);

        std::optional<float> flaps_voltage = _ads_1115_driver.getVoltage<config::remote_control::inputs::ads_1115::FLAPS_INPUT>();
        std::optional<float> roll_stab_voltage = _ads_1115_driver.getVoltage<config::remote_control::inputs::ads_1115::ROLL_STAB_INPUT>();
        std::optional<float> pitch_stab_voltage = _ads_1115_driver.getVoltage<config::remote_control::inputs::ads_1115::PITCH_STAB_INPUT>();

        read_error = flaps_voltage && roll_stab_voltage && pitch_stab_voltage;

        res.flaps_angle_deg = convertAdcReading(flaps_voltage.value_or(0), config::remote_control::inputs::FLAPS_ADC_CONFIG);
        res.roll_stabilization_level_percent = convertAdcReading(roll_stab_voltage.value_or(0), config::remote_control::inputs::ROLL_STAB_ADC_CONFIG);
        res.pitch_stabilization_level_percent = convertAdcReading(pitch_stab_voltage.value_or(0), config::remote_control::inputs::PITCH_STAB_ADC_CONFIG);

        return res;
    }

    dragonfly_msgs::msg::remote_control::InputsDiagnostics getDiagnostics()
    {
        dragonfly_msgs::msg::remote_control::InputsDiagnostics res;
        res.flaps_knob_state = _ads_1115_driver.getInputState<config::remote_control::inputs::ads_1115::FLAPS_INPUT>();
        res.roll_stab_knob_state = _ads_1115_driver.getInputState<config::remote_control::inputs::ads_1115::ROLL_STAB_INPUT>();
        res.pitch_stab_knob_state = _ads_1115_driver.getInputState<config::remote_control::inputs::ads_1115::PITCH_STAB_INPUT>();
        return res;
    }

private:

    float convertAdcReading(float voltage, const config::remote_control::inputs::AdcConfig& conf)
    {
        float ratio = std::clamp(
            (voltage - conf.min_voltage) / (conf.max_output - conf.min_voltage),
            0.0f,
            1.0f);
        
        return conf.inverted ? 
            conf.max_output + (conf.min_output - conf.max_output) * ratio :
            conf.min_output + (conf.max_output - conf.min_output) * ratio;
    }

    HardwareInterface _hw_interface;
    Ads1115Driver _ads_1115_driver;
};