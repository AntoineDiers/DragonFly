#pragma once

#include "InputGpioInterface.h"

#include <cstdint>

class InputGpioMock : public InputGpioInterface
{
public:

    InputGpioMock(){}

    void setState(bool state)
    {
        _state = state;
    }

    virtual bool read() override
    {
        return _state;
    }

private:

    bool _state;
};