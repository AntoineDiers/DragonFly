#pragma once

#include <algorithm>

#include <rpi_pico_utils/utils/Periodic.h>

#include <config/flight_controller/control_loop/Config.h>
#include <config/flight_controller/control_loop/pids/Config.h>

class Pid
{
public:

    Pid(const config::flight_controller::control_loop::pids::Config& config, ClockInterface* clock) : 
        _config(config),
        _clock(clock) {}

    float tick(float setpoint, float measure)
    {
        uint64_t now = _clock->getTime_us();

        float error, integral, derivate;

        error = measure - setpoint;

        if(_last_tick_info && _last_tick_info->getElapsed_us(_clock->getTime_us()) < _config.timeout_ms)
        {
            uint64_t dt_us = now - _last_tick_info->stamp_us;
            float dt_s = dt_us / 1000000.0;

            integral = std::clamp( _last_tick_info->value.integral + 0.5f * (_last_tick_info->value.error + error) * dt_s,
                    -_config.i_max, 
                    _config.i_max);

            if(dt_us < _config.min_interval_for_derivate_smoothing_us)
            {
                derivate = _last_tick_info->value.derivate;
            }
            else
            {   
                derivate = 
                    _config.derivate_smoothing_ratio * _last_tick_info->value.derivate + 
                    (1.0 - _config.derivate_smoothing_ratio) * (measure - _last_tick_info->value.measure) / dt_s;
            }
        }
        else
        {
            integral = 0;
            derivate = 0;
        }

        _last_tick_info = Stamped<LastTickInfo>
        {
            LastTickInfo 
            {
                .stamp = now,
                .measure = measure,
                .error = error,
                .derivate = derivate,
                .integral = integral
            },
            _clock->getTime_us()        
        };

        return - (_config.kp * error + _config.ki * integral + _config.kd * derivate);
    }

private:

    struct LastTickInfo
    {
        uint64_t stamp;
        float measure;
        float error;
        float derivate;
        float integral;
    };

    config::flight_controller::control_loop::pids::Config _config;
    ClockInterface* _clock;

    std::optional<Stamped<LastTickInfo>> _last_tick_info;
};

class Pids
{
public:

    Pids(ClockInterface* clock) : 
        roll(config::flight_controller::control_loop::pids::ROLL, clock),
        pitch(config::flight_controller::control_loop::pids::PITCH, clock),
        altitude(config::flight_controller::control_loop::pids::ALTITUDE, clock) {}

    Pid roll;
    Pid pitch;
    Pid altitude;
};