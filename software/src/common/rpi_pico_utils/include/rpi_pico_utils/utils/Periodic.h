#pragma once

#include "../clock/ClockInterface.h"
#include "Stamped.h"
//#include <dragonfly_msgs/msg/PeriodicDataState.h>

#include <optional>
#include <cmath>

struct PeriodicDataConfig
{
    float expected_rate_hz;
    float max_rate_deviation_hz;
    float rate_sliding_window_s;
};

struct PeriodicDataState
{
    bool is_ok;
    float expected_rate_hz;
    float actual_rate_hz;

    /*dragonfly_msgs::msg::PeriodicDataState toMsg()
    {
        return dragonfly_msgs::msg::PeriodicDataState 
        {
            .is_ok = is_ok,
            .expected_rate_integer_part_hz = (uint8_t) std::clamp((int)std::floor(expected_rate_hz), 0, 255),
            .expected_rate_decimal_part_hz = (uint8_t)(255 * (expected_rate_hz - std::floor(expected_rate_hz))),
            .actual_rate_integer_part_hz = (uint8_t) std::clamp((int)std::floor(actual_rate_hz), 0, 255),
            .actual_rate_decimal_part_hz = (uint8_t)(255 * (actual_rate_hz - std::floor(actual_rate_hz))),
        };
    }

    static PeriodicDataState fromMsg(const dragonfly_msgs::msg::PeriodicDataState& msg)
    {
        return PeriodicDataState
        {
            .is_ok = msg.is_ok,
            .expected_rate_hz = msg.expected_rate_integer_part_hz + msg.expected_rate_decimal_part_hz / 255.0f,
            .actual_rate_hz = msg.actual_rate_integer_part_hz + msg.actual_rate_decimal_part_hz / 255.0f,
        };
    }*/
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
        _mean_period_s = updateMeanPeriod();
        _last_value = {value, _clock->getTime_us()};
    }

    std::optional<Stamped<T>> getLast(const std::optional<uint32_t>& timeout_ms = std::nullopt) const
    {
        if(!_last_value) { return std::nullopt; }
        if(timeout_ms && _clock->getTime_us() - _last_value.stamp_us > timeout_ms.value() * 1000) { return std::nullopt; }
        return _last_value;
    }

    std::optional<T> getLastValue(const std::optional<uint32_t>& timeout_ms = std::nullopt) const
    {
        if(!_last_value) { return std::nullopt; }
        if(timeout_ms && _clock->getTime_us() - _last_value->stamp_us > timeout_ms.value() * 1000) { return std::nullopt; }
        return _last_value->value;
    }

    PeriodicDataState getState()
    {
        PeriodicDataState res;
        res.is_ok = false;
        res.expected_rate_hz = _config.expected_rate_hz;
        res.actual_rate_hz = 0;

        uint64_t now = _clock->getTime_us();

        if(!_last_value) { return res; }
        if(!_mean_period_s) { return res; }

        float time_since_last_s = _last_value->getElapsed_us(now) / 1000000.0;
        if(time_since_last_s > 2 / _config.expected_rate_hz) 
        {
            std::optional<float> mean_period_s = updateMeanPeriod();
            if(!mean_period_s) { return res; }
            res.actual_rate_hz = 1.0 / mean_period_s.value();
        }   
        else
        {
            res.actual_rate_hz = 1.0 / _mean_period_s.value();
        }

        res.is_ok = res.actual_rate_hz > _config.expected_rate_hz - _config.max_rate_deviation_hz && 
                    res.actual_rate_hz < _config.expected_rate_hz + _config.max_rate_deviation_hz;

        return res;
    }

private:

    std::optional<float> updateMeanPeriod()
    {
        if(!_last_value) { return std::nullopt; }

        uint64_t now = _clock->getTime_us();
        float time_since_last_s = _last_value->getElapsed_us(now) / 1000000.0;
        if(time_since_last_s > _config.rate_sliding_window_s) { return std::nullopt; }
        if(!_mean_period_s) { return time_since_last_s; }

        float weight = time_since_last_s / _config.rate_sliding_window_s;
        return  weight * time_since_last_s + (1.0 - weight) * _mean_period_s.value();    
    }

    PeriodicDataConfig _config;
    ClockInterface* _clock;

    std::optional<Stamped<T>> _last_value;
    std::optional<float> _mean_period_s;
};