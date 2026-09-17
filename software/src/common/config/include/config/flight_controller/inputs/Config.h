#pragma once

#include <rpi_pico_utils/utils/Periodic.h>
#include "../../../flight_controller/src/pressure_sensor_driver/PressureSensorDriver.h"

namespace config::flight_controller::inputs
{

    
    static constexpr PeriodicDataConfig REMOTE_CONTROL_RATE = 
    {
        .expected_rate_hz = 20,
        .max_rate_deviation_hz = 5,
        .rate_sliding_window_s = 1,
    };

    
    static constexpr PeriodicDataConfig PRIMARY_ATTITUDE_RATE = 
    {
        .expected_rate_hz = 100,
        .max_rate_deviation_hz = 10,
        .rate_sliding_window_s = 1,
    };

    
    static constexpr PeriodicDataConfig BACKUP_ATTITUDE_RATE = 
    {
        .expected_rate_hz = 100,
        .max_rate_deviation_hz = 10,
        .rate_sliding_window_s = 1,
    };

    
    static constexpr PeriodicDataConfig ALTITUDE_RATE = 
    {
        .expected_rate_hz = 20,
        .max_rate_deviation_hz = 5,
        .rate_sliding_window_s = 1,
    };

    static constexpr PeriodicDataConfig AIRSPEED_RATE = 
    {
        .expected_rate_hz = 10,
        .max_rate_deviation_hz = 3,
        .rate_sliding_window_s = 1,
    };

    static constexpr PeriodicDataConfig BATTERY_LEVEL_RATE = 
    {
        .expected_rate_hz = 10,
        .max_rate_deviation_hz = 3,
        .rate_sliding_window_s = 1,
    };

    static constexpr PeriodicDataConfig ADS1115_DATA_RATE = 
    {
        .expected_rate_hz = 20,
        .max_rate_deviation_hz = 5,
        .rate_sliding_window_s = 1,
    };

    static constexpr PeriodicDataConfig PRESSURE_DATA_RATE = 
    {
        .expected_rate_hz = 1000 / PressureSensorDriver::READ_PERIOD_MS,
        .max_rate_deviation_hz = 3,
        .rate_sliding_window_s = 1,
    };
}