#pragma once

#include <rpi_pico_utils/utils/Periodic.h>
#include <rpi_pico_utils/i2c/I2cImplementation.h>

#include "../../gpio/Config.h"

namespace config::remote_control::inputs::ads_1115
{
    I2cImplementation::Config I2C_CONFIG = 
    {
        .i2c_inst = i2c0,
        .scl_gpio = gpio::ADS_1115_SCL,
        .sda_gpio = gpio::ADS_1115_SDA,
        .addr = 0,
        .baudrate = 115200,
        .rw_timeout_ms = 3
    };

    enum class Input
    {
        A0 = 0,
        A1,
        A2,
        A3
    };

    static constexpr Input FLAPS_INPUT = Input::A0;
    static constexpr Input ROLL_STAB_INPUT = Input::A1;
    static constexpr Input PITCH_STAB_INPUT = Input::A2;

    static constexpr uint32_t TIME_BETWEEN_READS_MS = 10;
    static constexpr uint32_t CONVERSION_TIME_MS = 10;

    PeriodicDataConfig INPUTS_PERIOD_CONFIG = 
    {
        .timeout_ms = 100,
        .expected_rate_hz = 1000.0 / (4 * (TIME_BETWEEN_READS_MS + CONVERSION_TIME_MS)),
        .max_rate_deviation_hz = 3,
        .rate_smoothing_ratio = 0.9
    };
};