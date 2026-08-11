#pragma once

#include <cstdint>

namespace config::remote_control::inputs
{
    static constexpr uint32_t SEND_INPUTS_PERIOD_MS = 100; 

    static constexpr float MAX_FLAPS_ANGLE_DEG = 45;

    struct AdcConfig
    {
        float min_voltage;
        float max_voltage;
        float min_output;
        float max_output;
        bool inverted;
    };

    static constexpr AdcConfig LEFT_RIGHT_ADC_CONFIG =  { 0.0, 3.3, -1.0,   1.0, false };
    static constexpr AdcConfig DOWN_UP_ADC_CONFIG =     { 0.0, 3.3, -1.0,   1.0, false };
    static constexpr AdcConfig THROTTLE_ADC_CONFIG =    { 0.0, 3.3, 0,      100.0, false };
    static constexpr AdcConfig FLAPS_ADC_CONFIG =       { 0.0, 3.3, 0,      MAX_FLAPS_ANGLE_DEG, false};
    static constexpr AdcConfig ROLL_STAB_ADC_CONFIG =   { 0.0, 3.3, 0,      100.0, false };
    static constexpr AdcConfig PITCH_STAB_ADC_CONFIG =  { 0.0, 3.3, 0,      100.0, false };
};