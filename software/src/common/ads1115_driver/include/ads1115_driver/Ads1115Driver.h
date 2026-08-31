#pragma once

#include <rpi_pico_utils/io/IoInterface.h>
#include <rpi_pico_utils/clock/ClockInterface.h>
#include <rpi_pico_utils/timer/Timer.h>

#include <optional>
#include <array>

class Ads1115Driver
{
public:

    enum class Pin
    {
        A0 =0,
        A1,
        A2,
        A3
    };

    enum class SPS
    {
                            // Estimated read time (ms)
        SPS_8 =     0b000,  // 128
        SPS_16 =    0b001,  // 64
        SPS_32 =    0b010,  // 32
        SPS_64 =    0b011,  // 16
        SPS_128 =   0b100,  // 8
        SPS_250 =   0b101,  // 4
        SPS_475 =   0b110,  // 2
        SPS_860 =   0b111,  // 1
    };

    struct Reading
    {
        Pin pin;
        float voltage;
    };

    struct PinConfig
    {
        uint32_t read_period_ms;
        SPS sps;
    };

    struct Config
    {
        std::optional<PinConfig> a0_config;
        std::optional<PinConfig> a1_config;
        std::optional<PinConfig> a2_config;
        std::optional<PinConfig> a3_config;
    };

    struct PendingReadingInfo
    {
        Pin pin;  
        SPS sps;
        uint64_t read_start_stamp;
    };

    Ads1115Driver(ClockInterface* clock, IoInterface* io, const Config& config) :
        _clock(clock),
        _io(io),
        _config(config) 
    {
        if(_config.a0_config) { _a0_read_timer = Timer(_clock, _config.a0_config->read_period_ms); }
        if(_config.a1_config) { _a1_read_timer = Timer(_clock, _config.a1_config->read_period_ms); }
        if(_config.a2_config) { _a2_read_timer = Timer(_clock, _config.a2_config->read_period_ms); }
        if(_config.a3_config) { _a3_read_timer = Timer(_clock, _config.a3_config->read_period_ms); }
    }

    std::optional<Reading> tick()
    {
        if(!_pending_reading)
        {
            std::optional<PendingReadingInfo> next_reading;
            if(_a0_read_timer && _a0_read_timer->poll()) { next_reading = { Pin::A0, _config.a0_config->sps, _clock->getTime_us() }; }
            if(_a1_read_timer && _a1_read_timer->poll()) { next_reading = { Pin::A1, _config.a1_config->sps, _clock->getTime_us() }; }
            if(_a2_read_timer && _a2_read_timer->poll()) { next_reading = { Pin::A2, _config.a2_config->sps, _clock->getTime_us() }; }
            if(_a3_read_timer && _a3_read_timer->poll()) { next_reading = { Pin::A3, _config.a3_config->sps, _clock->getTime_us() }; }
            if(!next_reading) { return std::nullopt; }

            if(startReading(next_reading.value()))
            {
                _pending_reading = next_reading;
            }
        }

        if(!_pending_reading) { return std::nullopt; }

        // Read conversion state
        bool conversion_over = false;
        {
            uint8_t tx_buffer[1] = {0x01};
            uint8_t rx_buffer[2];
            conversion_over = write(tx_buffer, sizeof(tx_buffer)) && read(rx_buffer, sizeof(rx_buffer)) && (rx_buffer[0] & 0b10000000);
        }

        if(conversion_over)
        {
            std::optional<Reading> res;

            // Read conversion result
            uint8_t tx_buffer[1] = {0x00};
            uint8_t rx_buffer[2];
            if(write(tx_buffer, sizeof(tx_buffer)) && read(rx_buffer, sizeof(rx_buffer))) 
            { 
                int16_t raw = static_cast<int16_t>((static_cast<uint16_t>(rx_buffer[0]) << 8) + rx_buffer[1]);
                float voltage = raw * (4.096f / 32768.0f); 

                res = Reading
                {
                    .pin = _pending_reading->pin,
                    .voltage = voltage
                };
            }

            _pending_reading = std::nullopt;
            return res;
        }
        else
        {
            uint64_t time_since_reading_start_ms = (_clock->getTime_us() - _pending_reading->read_start_stamp) / 1000;
            if(time_since_reading_start_ms > 2 * spsToMs(_pending_reading->sps))
            {
                // Timeout
                _pending_reading = std::nullopt;
            }
        }

        return std::nullopt;
    }

    static uint32_t spsToMs(const SPS& sps)
    {
        switch (sps)
        {
            case SPS::SPS_8 :   { return 128; }
            case SPS::SPS_16 :  { return 64; }
            case SPS::SPS_32 :  { return 32; }
            case SPS::SPS_64 :  { return 16; }
            case SPS::SPS_128 : { return 8; }
            case SPS::SPS_250 : { return 4; }
            case SPS::SPS_475 : { return 2; }
            case SPS::SPS_860 : { return 1; }
            default:            { return 1; }
        }
    }

private:

    bool write(const uint8_t* buffer, uint32_t size)
    {
        uint32_t n_bytes_written;
        return _io->write(buffer, size, n_bytes_written) && n_bytes_written == size;
    }

    bool read(uint8_t* buffer, uint32_t size)
    {
        uint32_t n_bytes_read;
        return _io->read(buffer, size, n_bytes_read) && n_bytes_read == size;
    }

    bool startReading(const PendingReadingInfo& reading_infos)
    {
        // CONFIG :
        //
        // OS   = 1 : start conversion
        // MUX  = 1XX : AXX - GND
        // PGA  = 001 : +/- 4.096 V
        // MODE = 1 : single shot
        // DR   = YYY (SPS)
        // COMP_MODE = 0 : Traditional comparator
        // COMP_POL  = 0 : Active low
        // COMP_LAT  = 0 : Non-latching comparator
        // COMP_QUE  = 11 : Comparator disabled
        //
        // 11XX 0011 YYY0 0011

        uint8_t buffer[3] =
        {
            0x01,
            (uint8_t)(0xC3 + (((uint8_t)reading_infos.pin << 4) & 0b00110000)),
            (uint8_t)(0x03 + (((uint8_t)reading_infos.sps << 5) & 0b11100000)),
        };

        return write(buffer, sizeof(buffer));
    }

    ClockInterface* _clock;
    IoInterface* _io;
    Config _config;

    std::optional<Timer> _a0_read_timer;
    std::optional<Timer> _a1_read_timer;
    std::optional<Timer> _a2_read_timer;
    std::optional<Timer> _a3_read_timer;

    std::optional<PendingReadingInfo> _pending_reading;
};