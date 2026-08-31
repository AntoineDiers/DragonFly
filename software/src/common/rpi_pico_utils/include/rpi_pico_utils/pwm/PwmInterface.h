#pragma once

#include <cstdint>

class PwmInterface
{
public:

    virtual void setCommandPercent(float cmd_percent) = 0;
};