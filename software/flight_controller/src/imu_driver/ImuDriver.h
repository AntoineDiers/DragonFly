#pragma once

#include <math.h>

#include "../hardware/HardwareInterface.h"

#include <config/flight_controller/imu/Config.h>
#include <dragonfly_msgs/msgs.h>

class ImuDriver
{
public:

    ImuDriver(HardwareInterface hw_interface) : 
        _hw_interface(hw_interface)     
    {
        resetImu();
    }

    enum class State
    {
        SENDING_RESET_SIGNAL,
        WAITING_FOR_RESET,
        READY
    };

    std::optional<dragonfly_msgs::msg::Attitude> poll()
    {
        uint64_t now = _hw_interface.clock->getTime_us();

        if(_state == State::SENDING_RESET_SIGNAL)
        {
            uint64_t dt_ms = (now - _last_reset_stamp_us) / 1000;
            if(dt_ms > config::flight_controller::imu::RESET_SIGNAL_DURATION_MS)
            {
                _hw_interface.imu_rst_pin->set(true);
                _state = State::WAITING_FOR_RESET;
            }
            return std::nullopt;
        }
        else if(_state == State::WAITING_FOR_RESET)
        {
            if(!_hw_interface.imu_int_pin->read())
            {
                uint32_t n_bytes_read;
                _hw_interface.imu_i2c->read(_buffer, sizeof(_buffer), n_bytes_read);
            }            

            uint64_t dt_ms = (now - _last_reset_stamp_us) / 1000;
            if(dt_ms > config::flight_controller::imu::RESET_DURATION_MS)
            {
                if(sendInitCommand())
                {
                    _state = State::READY;
                    _last_valid_data_stamp_us = now;
                }
                else { resetImu();}
            }
            return std::nullopt;
        }
        else
        {
            uint64_t time_since_last_valid_data_ms = (now - _last_valid_data_stamp_us) / 1000;
            if(time_since_last_valid_data_ms > config::flight_controller::imu::GAME_ROTATION_VECTOR_TIMEOUT_MS)
            {
                resetImu();
                return std::nullopt;
            }

            if(_hw_interface.imu_int_pin->read()) { return std::nullopt; }

            uint32_t n_bytes_read;
            if(!_hw_interface.imu_i2c->read(_buffer, sizeof(_buffer), n_bytes_read) || n_bytes_read != sizeof(_buffer))
            {
                resetImu();
                return std::nullopt;
            }

            ShtpHeader header = parseShtpHeader(_buffer);
            
            // Ignore all packets except GAME_ROTATION_VECTOR
            if( header.packet_size != GAME_ROTATION_VECTOR_PACKET_SIZE || 
                header.channel != GAME_ROTATION_VECTOR_CHANNEL || 
                _buffer[GAME_ROTATION_VECTOR_INDEX] != GAME_ROTATION_VECTOR_ID)
            {
                return std::nullopt; 
            }
            
            _last_valid_data_stamp_us = now;

            // Parse the Game Rotation Vector
            return parseGameRotationVector(_buffer + GAME_ROTATION_VECTOR_INDEX);
        }
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
        _last_reset_stamp_us = _hw_interface.clock->getTime_us();
        _hw_interface.imu_rst_pin->set(false);
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

            0x10, 0x27, 0x00, 0x00,   // 10000 us

            0x00, 0x00, 0x00, 0x00,   // batch interval

            0x00, 0x00, 0x00, 0x00    // sensor specific
        };

        uint32_t n_bytes_written;
        return _hw_interface.imu_i2c->write(cmd, sizeof(cmd), n_bytes_written) && n_bytes_written == sizeof(cmd);
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

    dragonfly_msgs::msg::Attitude parseGameRotationVector(uint8_t* data)
    {
        constexpr float Q_SCALE = 1.0f / 16384.0f;
        constexpr float RAD_TO_DEG = 180.0f / M_PI;

        dragonfly_msgs::msg::Attitude res;

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

    static constexpr uint32_t SHTP_HEADER_SIZE = 4;
    static constexpr uint8_t GAME_ROTATION_VECTOR_CHANNEL = 3;
    static constexpr uint8_t GAME_ROTATION_VECTOR_ID = 8;
    static constexpr uint32_t GAME_ROTATION_VECTOR_PACKET_SIZE = 21;
    static constexpr uint32_t GAME_ROTATION_VECTOR_INDEX = 9;
    
    State _state = State::SENDING_RESET_SIGNAL;
    uint64_t _last_reset_stamp_us = 0;
    uint64_t _last_valid_data_stamp_us = 0;

    uint8_t _buffer[GAME_ROTATION_VECTOR_PACKET_SIZE];

    HardwareInterface _hw_interface;
};