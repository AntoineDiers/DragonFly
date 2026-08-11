#pragma once

#include <rpi_pico_utils/io/IoInterface.h>
#include <rpi_pico_utils/clock/ClockInterface.h>
#include <rpi_pico_utils/utils/Periodic.h>

#include <config/remote_control/inputs/ads_1115/Config.h>

#include <optional>

class Ads1115Driver
{
public:

    typedef config::remote_control::inputs::ads_1115::Input Input;

    Ads1115Driver(ClockInterface* clock, IoInterface* io) :
        _clock(clock),
        _io(io),
        _a0(config::remote_control::inputs::ads_1115::INPUTS_PERIOD_CONFIG, clock),
        _a1(config::remote_control::inputs::ads_1115::INPUTS_PERIOD_CONFIG, clock),  
        _a2(config::remote_control::inputs::ads_1115::INPUTS_PERIOD_CONFIG, clock),  
        _a3(config::remote_control::inputs::ads_1115::INPUTS_PERIOD_CONFIG, clock)
    {}

    void tick()
    {
        switch (_state.current_step)
        {
            case ReadSteps::IDLE:
            {
                if(timeSinceCurrentStepStart_ms() > config::remote_control::inputs::ads_1115::TIME_BETWEEN_READS_MS)
                {
                    nextStep();
                }
                break;
            }
            case ReadSteps::START_CONVERSION:
            {
                // CONFIG :
                //
                // OS   = 1 : start conversion
                // MUX  = 1XX : AXX - GND
                // PGA  = 001 : +/- 4.096 V
                // MODE = 1 : single shot
                // DR   = 100 : 128 SPS
                //
                // 11XX 0011 1000 0011
                // 0xC383

                std::array<uint8_t, 3> buffer =
                {
                    0x01,
                    (uint8_t)(0xC3 + (((uint8_t)_state.input << 4) & 0b00110000)),
                    0x83
                };

                if(write<3>(buffer)) { nextStep(); }
                else { nextInput(); }

                break;
            }
            case ReadSteps::WAIT_FOR_CONVERSION:
            {
                if(timeSinceCurrentStepStart_ms() > config::remote_control::inputs::ads_1115::CONVERSION_TIME_MS)
                {
                    nextStep();
                }
                break;
            }
            case ReadSteps::REQUEST_CONVERSION_STATE:
            {
                if(write<1>({0x01})) { nextStep(); }
                else { nextInput(); }

                break;
            }
            case ReadSteps::READ_CONVERSION_STATE:
            {
                std::array<uint8_t, 2> buffer;

                if(read<2>(buffer) && (buffer[0] & 0b10000000)) { nextStep(); }
                else { nextInput(); }
                
                break;
            }
            case ReadSteps::REQUEST_CONVERSION_RESULT:
            {
                if(write<1>({0x00})) { nextStep(); }
                else { nextInput(); }

                break;
            }
            case ReadSteps::READ_CONVERSION_RESULT:
            {
                std::array<uint8_t, 2> buffer;

                if(read<2>(buffer)) 
                {
                    int16_t raw = static_cast<int16_t>(
                        (static_cast<uint16_t>(buffer[0]) << 8) + buffer[1]);
                    float voltage = raw * (4.096f / 32768.0f);
                    switch (_state.input)
                    {
                        case Input::A0: _a0.update(voltage); break;
                        case Input::A1: _a1.update(voltage); break;
                        case Input::A2: _a2.update(voltage); break;
                        case Input::A3: _a3.update(voltage); break;
                    }
                };

                nextInput();
                
                break;

            }
        }
    }

    template<Input I>
    std::optional<float> getVoltage()
    {
        if constexpr (I == Input::A0) { return _a0.getLastValue(); }
        else if constexpr (I == Input::A1) { return _a1.getLastValue(); }
        else if constexpr (I == Input::A2) { return _a2.getLastValue(); }
        else if constexpr (I == Input::A3) { return _a3.getLastValue(); }
        else { static_assert(false); }
    }

    template<Input I>
    dragonfly_msgs::msg::PeriodicDataState getInputState()
    {
        if constexpr (I == Input::A0) { return _a0.getState(); }
        else if constexpr (I == Input::A1) { return _a1.getState(); }
        else if constexpr (I == Input::A2) { return _a2.getState(); }
        else if constexpr (I == Input::A3) { return _a3.getState(); }
        else { static_assert(false); }
    }

private:

    enum ReadSteps
    {
        IDLE,
        START_CONVERSION,
        WAIT_FOR_CONVERSION,
        REQUEST_CONVERSION_STATE,
        READ_CONVERSION_STATE,
        REQUEST_CONVERSION_RESULT,
        READ_CONVERSION_RESULT
    };

    struct State
    {
        ReadSteps current_step = ReadSteps::IDLE;
        uint64_t current_step_start = 0;
        config::remote_control::inputs::ads_1115::Input input = config::remote_control::inputs::ads_1115::Input::A0;
        
    };

    template<uint32_t Size>
    bool write(const std::array<uint8_t, Size>& buffer)
    {
        uint32_t n_bytes_written;
        return _io->canWrite() && _io->write(buffer.data(), Size, n_bytes_written) && n_bytes_written == Size;
    }

    template<uint32_t Size>
    bool read(std::array<uint8_t, Size>& buffer)
    {
        uint32_t n_bytes_read;
        return _io->read(buffer.data(), Size, n_bytes_read) && n_bytes_read == Size;
    }

    void nextStep()
    {
        if(_state.current_step == ReadSteps::READ_CONVERSION_RESULT)
        {
            nextInput();
        }
        else
        {
            _state.current_step = (ReadSteps)((uint8_t)_state.current_step + 1);
            _state.current_step_start = _clock->getTime_us();
        }
    }

    void nextInput()
    {
        if(_state.input == Input::A3)
        {
            _state.input = Input::A0;
        }
        else
        {
            _state.input = (Input)((uint8_t)_state.input + 1);
        }
        
        _state.current_step = ReadSteps::IDLE;
        _state.current_step_start = _clock->getTime_us();
    }

    uint32_t timeSinceCurrentStepStart_ms()
    {
        return (_clock->getTime_us() - _state.current_step_start) / 1000;
    }

    ClockInterface* _clock;
    IoInterface* _io;

    Periodic<float> _a0;
    Periodic<float> _a1;
    Periodic<float> _a2;
    Periodic<float> _a3;

    State _state;
};