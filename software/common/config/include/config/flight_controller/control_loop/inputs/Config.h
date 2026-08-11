#pragma once

#include <rpi_pico_utils/utils/Periodic.h>

namespace config::flight_controller::control_loop::inputs
{
    PeriodicDataConfig REMOTE_CONTROL = 
    {
        .timeout_ms = 200,
        .expected_rate_hz = 20,
        .max_rate_deviation_hz = 5,
        .rate_smoothing_ratio = 0.9,
    };

    PeriodicDataConfig PRIMARY_ATTITUDE = 
    {
        .timeout_ms = 200,
        .expected_rate_hz = 100,
        .max_rate_deviation_hz = 5,
        .rate_smoothing_ratio = 0.95,
    };

    PeriodicDataConfig BACKUP_ATTITUDE = 
    {
        .timeout_ms = 200,
        .expected_rate_hz = 20,
        .max_rate_deviation_hz = 5,
        .rate_smoothing_ratio = 0.9,
    };

    PeriodicDataConfig ALTITUDE = 
    {
        .timeout_ms = 200,
        .expected_rate_hz = 20,
        .max_rate_deviation_hz = 5,
        .rate_smoothing_ratio = 0.9,
    };

    PeriodicDataConfig BATTERY_LEVEL = 
    {
        .timeout_ms = 200,
        .expected_rate_hz = 20,
        .max_rate_deviation_hz = 5,
        .rate_smoothing_ratio = 0.9,
    };

}