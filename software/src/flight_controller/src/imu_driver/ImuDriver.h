#pragma once

#include <math.h>

#include "../inputs/FlightControllerInputs.h"

#include <rpi_pico_utils/gpio/InputGpioInterface.h>
#include <rpi_pico_utils/gpio/OutputGpioInterface.h>

#include <dragonfly_msgs/enums/DiagLevel.h>

class ImuDriver
{
public:

    enum class State
    {
        SENDING_RESET_SIGNAL,
        WAITING_FOR_RESET,
        READY
    };

    struct Config
    {
        uint32_t reset_signal_duration_ms;
        uint32_t reset_duration_ms;
        uint32_t game_rotation_vector_timeout_ms;
        uint32_t read_period_ms;
    };

    ImuDriver(
        const Config& config,
        Periodic<FlightControllerInputs::Attitude>* output, 
        ClockInterface* clock,
        IoInterface* i2c, 
        InputGpioInterface* int_pin, 
        OutputGpioInterface* rst_pin) : 
        _config(config),
        _output(output),
        _clock(clock),
        _i2c(i2c),
        _int_pin(int_pin),
        _rst_pin(rst_pin)
    {
        resetImu();
    }

    static constexpr uint32_t SHTP_HEADER_SIZE = 4;
    static constexpr uint8_t GAME_ROTATION_VECTOR_CHANNEL = 3;
    static constexpr uint8_t GAME_ROTATION_VECTOR_ID = 8;
    static constexpr uint32_t GAME_ROTATION_VECTOR_PACKET_SIZE = 21;
    static constexpr uint32_t GAME_ROTATION_VECTOR_INDEX = 9;
    static constexpr float Q_SCALE = 1.0f / 16384.0f;
    static constexpr float RAD_TO_DEG = 180.0f / M_PI;

    void tick()
    {
        uint64_t now = _clock->getTime_us();

        if(_state == State::SENDING_RESET_SIGNAL)
        {
            uint64_t dt_ms = (now - _last_reset_stamp_us) / 1000;
            if(dt_ms > _config.reset_signal_duration_ms)
            {
                _rst_pin->set(true);
                _state = State::WAITING_FOR_RESET;
            }
            return;
        }
        else if(_state == State::WAITING_FOR_RESET)
        {
            if(!_int_pin->read())
            {
                uint32_t n_bytes_read;
                _i2c->read(_buffer, sizeof(_buffer), n_bytes_read);
            }            

            uint64_t dt_ms = (now - _last_reset_stamp_us) / 1000;
            if(dt_ms > _config.reset_duration_ms)
            {
                if(sendInitCommand())
                {
                    _state = State::READY;
                    _last_valid_data_stamp_us = now;
                }
                else { resetImu();}
            }
            return;
        }
        else
        {
            uint64_t time_since_last_valid_data_ms = (now - _last_valid_data_stamp_us) / 1000;
            if(time_since_last_valid_data_ms > _config.game_rotation_vector_timeout_ms)
            {
                resetImu();
                return;
            }

            if(_int_pin->read()) { return; }

            uint32_t n_bytes_read;
            if(!_i2c->read(_buffer, sizeof(_buffer), n_bytes_read) || n_bytes_read != sizeof(_buffer))
            {
                return;
            }

            ShtpHeader header = parseShtpHeader(_buffer);
            
            // Ignore all packets except GAME_ROTATION_VECTOR
            if( header.packet_size != GAME_ROTATION_VECTOR_PACKET_SIZE || 
                header.channel != GAME_ROTATION_VECTOR_CHANNEL || 
                _buffer[GAME_ROTATION_VECTOR_INDEX] != GAME_ROTATION_VECTOR_ID)
            {
                return; 
            }
            
            _last_valid_data_stamp_us = now;

            // Parse the Game Rotation Vector
            _output->update(parseGameRotationVector(_buffer + GAME_ROTATION_VECTOR_INDEX));
        }
    }

    dragonfly_msgs::enums::DiagLevel getDiagLevel()
    {
        return _output->getState().is_ok ? dragonfly_msgs::enums::DiagLevel::OK : dragonfly_msgs::enums::DiagLevel::ERR;
    }

private:

    struct ShtpHeader
    {
        uint16_t packet_size;
        bool continuation_bit;
        uint8_t channel;
        uint8_t sequence;
    };

    void resetImu()
    {
        _state = State::SENDING_RESET_SIGNAL;
        _last_reset_stamp_us = _clock->getTime_us();
        _rst_pin->set(false);
    }

    bool sendInitCommand()
    {
        uint8_t cmd[] =
        {
            0x15, 0x00,      // 21 = 4(header)+17(payload)
            0x02,
            0x00,

            0xFD,
            0x08,

            0x00,            // Feature flags

            0x00, 0x00,      // Change sensitivity

            0x00, 0x00, 0x00, 0x00,   // read period

            0x00, 0x00, 0x00, 0x00,   // batch interval

            0x00, 0x00, 0x00, 0x00    // sensor specific
        };

        uint64_t period_us = _config.read_period_ms * 1000;
        cmd[9] = (period_us >> 0) & 0xFF;
        cmd[10] = (period_us >> 8) & 0xFF;
        cmd[11] = (period_us >> 16) & 0xFF;
        cmd[12] = (period_us >> 24) & 0xFF;

        uint32_t n_bytes_written;
        bool res = _i2c->write(cmd, sizeof(cmd), n_bytes_written) && n_bytes_written == sizeof(cmd);
        
        return res; 
    }

    ShtpHeader parseShtpHeader(uint8_t* data)
    {
        ShtpHeader res;
        res.packet_size = ((data[1] & 0b01111111) << 8) | data[0];
        res.continuation_bit = data[1] & 0b10000000;
        res.channel = data[2];
        res.sequence = data[3];
        return res;
    }

    FlightControllerInputs::Attitude parseGameRotationVector(uint8_t* data)
    {
        FlightControllerInputs::Attitude res;

        // Extraction quaternion (little endian)
        float x = (int16_t)(data[4]  | (data[5]  << 8)) * Q_SCALE;
        float y = (int16_t)(data[6]  | (data[7]  << 8)) * Q_SCALE;
        float z = (int16_t)(data[8]  | (data[9] << 8)) * Q_SCALE;
        float w = (int16_t)(data[10] | (data[11] << 8)) * Q_SCALE;

        // Normalise quaternion
        float norm = sqrtf(x*x + y*y + z*z + w*w);
        x /= norm;
        y /= norm;
        z /= norm;
        w /= norm;

        // Roll
        float sinRoll = 2.0f * (w * x + y * z);
        float cosRoll = 1.0f - 2.0f * (x * x + y * y);
        res.roll_deg = RAD_TO_DEG * atan2f(sinRoll, cosRoll);

        // Pitch
        float sinPitch = std::clamp(2.0f * (w * y - z * x), -1.0f, 1.0f);
        res.pitch_deg = RAD_TO_DEG * asinf(sinPitch);

        return res;
    };

    Config _config;
    
    State _state = State::SENDING_RESET_SIGNAL;
    uint64_t _last_reset_stamp_us = 0;
    uint64_t _last_valid_data_stamp_us = 0;

    uint8_t _buffer[GAME_ROTATION_VECTOR_PACKET_SIZE];

    Periodic<FlightControllerInputs::Attitude>* _output;
    ClockInterface* _clock;
    IoInterface* _i2c;
    InputGpioInterface* _int_pin; 
    OutputGpioInterface* _rst_pin;
};