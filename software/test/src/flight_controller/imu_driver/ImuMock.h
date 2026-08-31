#pragma once

#include <rpi_pico_utils/io/IoInterface.h>
#include <rpi_pico_utils/gpio/InputGpioMock.h>
#include <rpi_pico_utils/gpio/OutputGpioMock.h>
#include <rpi_pico_utils/timer/Timer.h>

#include "../../../src/flight_controller/src/inputs/FlightControllerInputs.h"
#include "../../../src/flight_controller/src/imu_driver/ImuDriver.h"

#include <optional>
#include <iostream>

class ImuMock : public IoInterface
{
public:

    ImuMock(ClockInterface* clock) : 
        _clock(clock),
        _data_ready_timer(clock, 5) 
    {
        _boot_time = clock->getTime_us();
    }

    void poll()
    {
        if(!_rst_pin.getState() && _state != State::WAITING_FOR_COMMAND) 
        { 
            _n_resets++;
            _state = State::WAITING_FOR_COMMAND;
            _attitude_ready_timer = std::nullopt; 
        }
        if(_data_ready_timer.poll()) 
        {
            _int_pin.setState(false); 
        }
    }

    void setAttitude(const std::optional<FlightControllerInputs::Attitude>& attitude)
    {
        _attitude = attitude;
    }

    bool read(uint8_t *data, uint32_t n_bytes_to_read, uint32_t& n_bytes_read) override
    {
        if(!_int_pin.read() && _state == State::RUNNING && _attitude_ready_timer)
        {
            _int_pin.setState(true);
            if( _attitude_ready_timer->poll() && 
                _state == State::RUNNING &&
                n_bytes_to_read >= ImuDriver::GAME_ROTATION_VECTOR_PACKET_SIZE && 
                _attitude.has_value())
            {
                // SHTP Header
                data[0] = ImuDriver::GAME_ROTATION_VECTOR_PACKET_SIZE & 0xFF;
                data[1] = 0;
                data[2] = ImuDriver::GAME_ROTATION_VECTOR_CHANNEL;
                data[3] = 0;

                data[ImuDriver::GAME_ROTATION_VECTOR_INDEX] = ImuDriver::GAME_ROTATION_VECTOR_ID;

                double cr = std::cos(_attitude->roll_deg * M_PI / 360);
                double sr = std::sin(_attitude->roll_deg * M_PI / 360);
                double cp = std::cos(_attitude->pitch_deg * M_PI / 360);
                double sp = std::sin(_attitude->pitch_deg * M_PI / 360);
                double cy = std::cos(0 * M_PI / 360);
                double sy = std::sin(0 * M_PI / 360);

                int16_t w = (cr * cp * cy + sr * sp * sy) / ImuDriver::Q_SCALE;
                int16_t x = (sr * cp * cy + cr * sp * sy) / ImuDriver::Q_SCALE;
                int16_t y = (cr * sp * cy + sr * cp * sy) / ImuDriver::Q_SCALE;
                int16_t z = (cr * cp * sy + sr * sp * cy) / ImuDriver::Q_SCALE;

                data[ImuDriver::GAME_ROTATION_VECTOR_INDEX + 4] = x & 0xFF;
                data[ImuDriver::GAME_ROTATION_VECTOR_INDEX + 5] = (x >> 8) & 0xFF;
                data[ImuDriver::GAME_ROTATION_VECTOR_INDEX + 6] = y & 0xFF;
                data[ImuDriver::GAME_ROTATION_VECTOR_INDEX + 7] = (y >> 8) & 0xFF;
                data[ImuDriver::GAME_ROTATION_VECTOR_INDEX + 8] = z & 0xFF;
                data[ImuDriver::GAME_ROTATION_VECTOR_INDEX + 9] = (z >> 8) & 0xFF;
                data[ImuDriver::GAME_ROTATION_VECTOR_INDEX + 10] = w & 0xFF;
                data[ImuDriver::GAME_ROTATION_VECTOR_INDEX + 11] = (w >> 8) & 0xFF;

                n_bytes_read = ImuDriver::GAME_ROTATION_VECTOR_PACKET_SIZE;

                return true;
            }
            else 
            {
                n_bytes_read = rand() % (n_bytes_to_read + 1);
                for(int i = 0; i < n_bytes_read; i++)
                {
                    data[i] = rand() & 0xFF;
                }
                return true;
            }
        }
        return false;
    }

    bool write(const uint8_t *data, uint32_t n_bytes_to_write, uint32_t& n_bytes_written) override
    {

        if(_state != State::WAITING_FOR_COMMAND) { return false; }

        uint64_t dt_ms = (_clock->getTime_us() - _boot_time) / 1000;
        if(dt_ms < 500) { return false; }

        uint8_t expected_cmd[] =
        {
            0x15, 0x00,      // 21 = 4(header)+17(payload)
            0x02,
            0x00,

            0xFD,
            0x08,

            0x00,            // Feature flags

            0x00, 0x00,      // Change sensitivity

            0x00, 0x00, 0x00, 0x00,   // read period us

            0x00, 0x00, 0x00, 0x00,   // batch interval

            0x00, 0x00, 0x00, 0x00    // sensor specific
        };
        
        if(n_bytes_to_write != sizeof(expected_cmd)) { return false; }

        bool is_expected_command = true;
        for(uint32_t i = 0; i < n_bytes_to_write; i++)
        {
            if(expected_cmd[i] != data[i] && (i < 9 || i > 12))
            {
                is_expected_command = false;
            }
        }

        if(!is_expected_command) { return false; }

        uint64_t read_period_us = data[9] + (data[10] << 8) + (data[11] << 16) + (data[12] << 24);
        _attitude_ready_timer = Timer(_clock, read_period_us / 1000);

        n_bytes_written = n_bytes_to_write;
        _state = State::RUNNING;
        return true;
    }

    InputGpioMock* intPin() { return &_int_pin; }
    OutputGpioMock* rstPin() { return &_rst_pin; }

    uint32_t resetCount() { return _n_resets; }

private:

    enum State
    {
        WAITING_FOR_COMMAND,
        RUNNING
    };

    ClockInterface* _clock;
    InputGpioMock _int_pin;
    OutputGpioMock _rst_pin; 

    uint64_t _boot_time;
    std::optional<FlightControllerInputs::Attitude> _attitude = std::nullopt;

    State _state = State::WAITING_FOR_COMMAND;
    uint32_t _n_resets = 0;

    Timer _data_ready_timer;
    std::optional<Timer> _attitude_ready_timer;
};