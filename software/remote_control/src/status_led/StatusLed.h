#pragma once

#include "../hardware/HardwareInterface.h"

#include <config/remote_control/leds/Config.h>
#include <rpi_pico_utils/timer/Timer.h>

class StatusLed
{
public:

    enum class Status
    {
        OK,
        WARN,
        ERR
    };

    StatusLed(ClockInterface* clock, OutputGpioInterface* red_gpio, OutputGpioInterface* green_gpio) :
        _blink_timer(clock, config::remote_control::leds::LED_BLINK_PERIOD_MS),
        _red_gpio(red_gpio),
        _green_gpio(green_gpio)
    {
        updateLed();
    }

    void setStatus(const Status& status)
    {
        if(status != _status)
        {
            _status = status;
            updateLed();
        }
    }

    void tick()
    {
        if(_blink_timer.poll())
        {
            updateLed();
            _blink_state = !_blink_state;
        }
    }

private:

    void updateLed()
    {
        switch (_status)
        {
            case Status::OK:
            {
                _red_gpio->set(false);
                _green_gpio->set(true);
                break;
            }
            case Status::WARN:
            {
                _red_gpio->set(_blink_state);
                _green_gpio->set(_blink_state);
                break;
            }
            case Status::ERR:
            {
                _red_gpio->set(true);
                _green_gpio->set(false);
                break;
            }
        }
    }

    Timer _blink_timer;
    OutputGpioInterface* _red_gpio;
    OutputGpioInterface* _green_gpio;
    
    Status _status = Status::ERR;
    bool _blink_state = true;
};