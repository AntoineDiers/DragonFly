#pragma once

#include "OutputGpioInterface.h"

#include <cstdint>

class OutputGpioMock : public OutputGpioInterface
{
public:

    OutputGpioMock(){}

    bool getState() { return _state; }

    virtual void set(bool val) override
    {
        _state = val;
    }

private:

    bool _state = false;
};