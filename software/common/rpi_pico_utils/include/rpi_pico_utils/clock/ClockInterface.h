#pragma once

#include <cstdint>

class ClockInterface
{
public:

    virtual uint64_t getTime_us() const = 0;
};