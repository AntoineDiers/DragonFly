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

    StatusLed(ClockInterface* clock, PwmInterface* red_gpio, PwmInterface* green_gpio) :
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
                _red_gpio->setCommandPercent(0);
                _green_gpio->setCommandPercent(0.1 * config::remote_control::leds::GREEN_INTENSITY_PERCENT);
                break;
            }
            case Status::WARN:
            {
                _red_gpio->setCommandPercent(_blink_state ? config::remote_control::leds::RED_INTENSITY_PERCENT : 0);
                _green_gpio->setCommandPercent(_blink_state ? config::remote_control::leds::GREEN_INTENSITY_PERCENT : 0);
                break;
            }
            case Status::ERR:
            {
                _red_gpio->setCommandPercent(_blink_state ? config::remote_control::leds::RED_INTENSITY_PERCENT : 0);
                _green_gpio->setCommandPercent(0);
                break;
            }
        }
    }

    Timer _blink_timer;
    PwmInterface* _red_gpio;
    PwmInterface* _green_gpio;
    
    Status _status = Status::ERR;
    bool _blink_state = true;
};