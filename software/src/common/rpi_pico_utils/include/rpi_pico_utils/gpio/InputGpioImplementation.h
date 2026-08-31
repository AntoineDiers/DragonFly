#pragma once

#include "InputGpioInterface.h"

#include <cstdint>

#include <hardware/gpio.h>

class InputGpioImplementation : public InputGpioInterface
{
public:

    InputGpioImplementation(uint8_t gpio) : 
        _gpio(gpio)
    {
        gpio_init(_gpio);
        gpio_set_dir(_gpio, GPIO_IN);
        gpio_pull_up(_gpio);
    }

    virtual bool read() override
    {
        return gpio_get(_gpio);
    }

private:

    uint8_t _gpio;
};