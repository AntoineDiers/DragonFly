#pragma once

#include "../clock/ClockInterface.h"

class Timer
{
public:

    Timer(ClockInterface* clock, uint32_t period_ms) : 
        _clock(clock),
        _period_us(1000 * period_ms),
        _next_tick_index(1),
        _start_us(clock->getTime_us()) {};

    bool poll()
    {
        uint64_t now = _clock->getTime_us();
        uint64_t next_tick_us = _start_us + _period_us * _next_tick_index;

        if(now < next_tick_us) { return false; }

        _next_tick_index = 1 + (now - _start_us) / _period_us;

        return true;
    }

private:

    ClockInterface* _clock;
    uint64_t _period_us;
    uint64_t _next_tick_index;
    uint64_t _start_us;
};