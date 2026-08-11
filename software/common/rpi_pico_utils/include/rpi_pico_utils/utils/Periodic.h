#pragma once

#include "../clock/ClockInterface.h"
#include "Stamped.h"

#include <dragonfly_msgs/msg/PeriodicDataState.h>

#include <optional>
#include <cmath>

struct PeriodicDataConfig
{
    uint32_t timeout_ms;
    float expected_rate_hz;
    float max_rate_deviation_hz;
    float rate_smoothing_ratio;
};

enum class PeriodicDataState
{
    OK = 0,
    UNEXPECTED_RATE = 1,
    TIMEOUT = 2,
};

template<typename T>
class Periodic
{
public: 

    Periodic(const PeriodicDataConfig& config, ClockInterface* clock) : 
        _config(config),
        _clock(clock) {}

    void update(const T& value)
    {
        uint64_t now = _clock->getTime_us();

        std::optional<Stamped<T>> last_value = getLast();
        if(last_value)
        {
            _mean_period_s = 
                _config.rate_smoothing_ratio * _mean_period_s + 
                (1.0f - _config.rate_smoothing_ratio) * ((now - last_value->stamp_us) / 1000000.0f); 
        }
        _value = { value, now };
    }

    std::optional<Stamped<T>> getLast() 
    {
        if(!_value) { return std::nullopt; }
        if(_clock->getTime_us() - _value->stamp_us > _config.timeout_ms * 1000) { return std::nullopt; }
        return _value;
    }

    std::optional<T> getLastValue() 
    {
        if(!_value) { return std::nullopt; }
        if(_clock->getTime_us() - _value->stamp_us > _config.timeout_ms * 1000) { return std::nullopt; }
        return _value->value;
    }

    dragonfly_msgs::msg::PeriodicDataState getState()
    {
        dragonfly_msgs::msg::PeriodicDataState res;
        res.state = (uint8_t)PeriodicDataState::TIMEOUT;
        res.rate_hz = 0;
        res.time_since_last_ms = 0;

        std::optional<Stamped<T>> last_value = getLast();
        if(last_value) 
        {
            res.time_since_last_ms = (_clock->getTime_us() - last_value->stamp_us) / 1000;
            res.rate_hz = 1.0f / _mean_period_s;

            if( std::isnan(res.rate_hz) || 
                res.rate_hz > _config.expected_rate_hz + _config.max_rate_deviation_hz ||
                res.rate_hz < _config.expected_rate_hz - _config.max_rate_deviation_hz)
            {
                res.state = (uint8_t)PeriodicDataState::UNEXPECTED_RATE;
            }
            else
            {
                res.state = (uint8_t)PeriodicDataState::OK;
            }
        }

        return res;
    }

    float getRate()
    {
        return 1.0f / _mean_period_s;
    }

private:

    PeriodicDataConfig _config;
    ClockInterface* _clock;
    std::optional<Stamped<T>> _value;
    float _mean_period_s = 0;
};