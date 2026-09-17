#pragma once

#include <rpi_pico_utils/clock/ClockInterface.h>
#include <optional>
#include <dragonfly_msgs/enums/ErrorState.h>

#include <config/common/debug/Config.h>
#include <iostream>

class Event
{
public:

    enum class Criticity
    {
        INFO,
        WARNING,
        ERROR
    };

    template<uint32_t MsgSize>
    Event(ClockInterface* clock, const char (&msg)[MsgSize], uint32_t min_print_period_ms, Criticity criticity) :
        _clock(clock),
        _min_print_period_ms(min_print_period_ms),
        _criticity(criticity)
    {
        static_assert(MsgSize <= MAX_MSG_SIZE);
        std::memcpy(_msg, msg, MsgSize);
    }

    void trigger()
    {
        _last_trigger = _clock->getTime_us();
        if constexpr (config::common::debug::DEBUG)
        {
            if(_last_print.has_value())
            {
                uint32_t dt_ms = (_last_trigger.value() - _last_print.value()) / 1000;
                if(dt_ms < _min_print_period_ms)
                {
                    return;
                }
            }

            _last_print = _clock->getTime_us();

            std::string debug_str;
            switch (_criticity)
            {
                case Criticity::INFO:       { debug_str = "\033[36m[INFO]  "; break; }
                case Criticity::WARNING:    { debug_str = "\033[33m[WARN]  "; break; }
                case Criticity::ERROR:      { debug_str = "\033[31m[ERROR] "; break; }
            }

            debug_str += _msg;
            debug_str += "\033[0m";

            std::cout << debug_str << "\n";
        }
    }

    dragonfly_msgs::enums::ErrorState getState()
    {
        if(!_last_trigger) { return dragonfly_msgs::enums::ErrorState::NEVER_TRIGGERED; }

        double dt_s = (_clock->getTime_us() - _last_trigger.value()) / 1000000.0;
        if(dt_s < 1) { return dragonfly_msgs::enums::ErrorState::TRIGGERED_LESS_THAN_1_S_AGO; }
        else if(dt_s < 30) { return dragonfly_msgs::enums::ErrorState::TRIGGERED_LESS_THAN_30_S_AGO; }
        else { return dragonfly_msgs::enums::ErrorState::TRIGGERED_MORE_THAN_30_S_AGO; } 
    }

private:

    static constexpr uint32_t MAX_MSG_SIZE = 50;

    ClockInterface* _clock;
    std::optional<uint64_t> _last_trigger;
    std::optional<uint64_t> _last_print;
    uint32_t _min_print_period_ms;
    Criticity _criticity;
    char _msg[MAX_MSG_SIZE];
};