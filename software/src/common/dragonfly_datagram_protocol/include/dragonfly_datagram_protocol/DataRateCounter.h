#pragma once

#include <rpi_pico_utils/timer/Timer.h>

class DataRateCounter
{
public:

    DataRateCounter(ClockInterface* clock) :
        _timer(clock, HISTORY_ENTRY_DURATION_MS) {}

    void tick()
    {
        if(_timer.poll())
        {
            for(uint32_t i = 0; i < (HISTORY_SIZE - 1); i++)
            {
                _history[i] = _history[i+1];
            }
            _history[HISTORY_SIZE - 1] = 0;
        }
    }

    void registerData(uint32_t n_bytes)
    {
        _history[HISTORY_SIZE - 1] += n_bytes;
    }

    double getDataRate_bytes_per_s()
    {
        uint32_t n_bytes = 0;
        uint32_t dt_ms = 0;
        for(uint32_t i = 0; i < (HISTORY_SIZE - 1); i++)
        {
            n_bytes += _history[i];
            dt_ms += HISTORY_ENTRY_DURATION_MS;
        }
        return n_bytes / (0.001 * dt_ms);
    }

private:

    static constexpr uint32_t HISTORY_SIZE = 10;
    static constexpr uint32_t HISTORY_ENTRY_DURATION_MS = 100;

    Timer _timer;
    uint32_t _history[HISTORY_SIZE] = {0};
};