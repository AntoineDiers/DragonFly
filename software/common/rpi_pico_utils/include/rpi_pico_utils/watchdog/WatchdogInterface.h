#pragma once

#include <cstdint>

class WatchdogInterface
{
public:

    virtual void update() = 0;
    virtual uint32_t restartsCount() = 0;
};