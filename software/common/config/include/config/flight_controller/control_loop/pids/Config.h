#pragma once

#include <cstdint>

namespace config::flight_controller::control_loop::pids
{
    static constexpr uint32_t MIN_INTERVAL_FOR_DERIVATIVE_US = 10000;

    struct Config
    {
        float kp, ki, kd;
        float i_max;
        float derivate_smoothing_ratio;
        uint32_t timeout_ms;
        uint64_t min_interval_for_derivate_smoothing_us;
    };

    static constexpr Config ROLL = 
    {
        .kp = 1.0,
        .ki = 0.1,
        .kd = 0.1,
        .i_max = 10,
        .derivate_smoothing_ratio = 0.9,
        .timeout_ms = 100,
        .min_interval_for_derivate_smoothing_us = 1000
    };

    static constexpr Config PITCH = 
    {
        .kp = 1.0,
        .ki = 0.1,
        .kd = 0.1,
        .i_max = 10,
        .derivate_smoothing_ratio = 0.9,
        .timeout_ms = 100,
        .min_interval_for_derivate_smoothing_us = 1000
    };

    static constexpr Config ALTITUDE = 
    {
        .kp = 1.0,
        .ki = 0.1,
        .kd = 0.1,
        .i_max = 10,
        .derivate_smoothing_ratio = 0.9,
        .timeout_ms = 100,
        .min_interval_for_derivate_smoothing_us = 1000
    };
}