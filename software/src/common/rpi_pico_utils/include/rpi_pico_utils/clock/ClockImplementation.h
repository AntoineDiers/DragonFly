#pragma once

#include "ClockInterface.h"

#include <pico/time.h>

class ClockImplementation : public ClockInterface
{
public:

    virtual uint64_t getTime_us() const override
    {
        return time_us_64();
    };
};