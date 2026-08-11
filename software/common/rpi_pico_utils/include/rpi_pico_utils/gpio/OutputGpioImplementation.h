#pragma once

#include "OutputGpioInterface.h"

#include <cstdint>

#include <hardware/gpio.h>

class OutputGpioImplementation : public OutputGpioInterface
{
public:

    OutputGpioImplementation(uint8_t gpio) : 
        _gpio(gpio)
    {
        gpio_init(_gpio);
        gpio_set_dir(_gpio, GPIO_OUT);
    }

    virtual void set(bool val) override
    {
        gpio_put(_gpio, val);
    }

private:

    uint8_t _gpio;
};