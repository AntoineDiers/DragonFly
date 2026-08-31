#pragma once

#include <cstdint>

class WatchdogInterface
{
public:

    virtual void update() = 0;
};