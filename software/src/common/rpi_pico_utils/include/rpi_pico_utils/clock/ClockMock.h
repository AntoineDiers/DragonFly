#pragma once

#include "ClockInterface.h"

class ClockMock : public ClockInterface
{
public:

    virtual uint64_t getTime_us() const override
    {
        return _time;
    };

    void setTime(uint64_t time)
    {
        _time = time;
    }

    void advance_ms(uint32_t dt_ms)
    {
        _time += 1000 * dt_ms;
    };

private:

    uint64_t _time = 0;
};