#pragma once

#include <rpi_pico_utils/utils/Periodic.h>
#include <rpi_pico_utils/i2c/I2cImplementation.h>
#include <ads1115_driver/Ads1115Driver.h>

#include "../../gpio/Config.h"

namespace config::remote_control::inputs::ads_1115
{
    static constexpr uint8_t SLAVE_ADDR = 0x48; 

    static constexpr Ads1115Driver::Pin ROLL_STAB_PIN = Ads1115Driver::Pin::A0;
    static constexpr Ads1115Driver::Pin PITCH_STAB_PIN = Ads1115Driver::Pin::A1;
    static constexpr Ads1115Driver::Pin FLAPS_PIN = Ads1115Driver::Pin::A2;
    
    uint32_t READ_PERIOD_MS = 100;
    Ads1115Driver::Config CONFIG = 
    {
        .a0_config = Ads1115Driver::PinConfig { .read_period_ms = READ_PERIOD_MS, .sps = Ads1115Driver::SPS::SPS_32 },
        .a1_config = Ads1115Driver::PinConfig { .read_period_ms = READ_PERIOD_MS, .sps = Ads1115Driver::SPS::SPS_32 },
        .a2_config = Ads1115Driver::PinConfig { .read_period_ms = READ_PERIOD_MS, .sps = Ads1115Driver::SPS::SPS_32 },
        .a3_config = std::nullopt
    };

    uint32_t INPUTS_TIMEOUT_MS = 250;
    PeriodicDataConfig INPUTS_RATE = 
    {
        .expected_rate_hz = 1000.0f / (3 * READ_PERIOD_MS),
        .max_rate_deviation_hz = 3,
        .rate_sliding_window_s = 1
    };
};