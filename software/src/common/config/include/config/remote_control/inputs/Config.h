#pragma once

#include <cstdint>
#include <optional>

namespace config::remote_control::inputs
{
    static constexpr uint32_t SEND_INPUTS_PERIOD_MS = 100; 

    static constexpr float MAX_FLAPS_ANGLE_DEG = 45;

    struct AdcConfig
    {
        float min_voltage;
        float max_voltage;
        bool inverted;
        std::optional<float> center_dead_zone;
    };

    static constexpr AdcConfig LEFT_RIGHT_ADC_CONFIG =  { 1.3,  2.7,    true,   0.05 };
    static constexpr AdcConfig DOWN_UP_ADC_CONFIG =     { 0.5,  2.9,    false,  0.05 };
    static constexpr AdcConfig THROTTLE_ADC_CONFIG =    { 0.5,  2.9,    true,   std::nullopt };
    static constexpr AdcConfig FLAPS_ADC_CONFIG =       { 0.01, 3.2,    true,   std::nullopt };
    static constexpr AdcConfig ROLL_STAB_ADC_CONFIG =   { 0.01, 3.2,    true,   std::nullopt };
    static constexpr AdcConfig PITCH_STAB_ADC_CONFIG =  { 0.01, 3.2,    true,   std::nullopt };
};