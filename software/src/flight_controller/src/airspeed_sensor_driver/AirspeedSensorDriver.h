#pragma once

#include <rpi_pico_utils/io/IoInterface.h>
#include <rpi_pico_utils/timer/Timer.h>
#include <rpi_pico_utils/utils/Periodic.h>

#include <dragonfly_msgs/enums/DiagLevel.h>

class AirspeedSensorDriver
{
public:

    struct Config
    {
        uint32_t read_period_ms;
    };

    AirspeedSensorDriver(const Config& config, ClockInterface* clock, IoInterface* i2c, Periodic<float>* output_km_h) :
        _clock(clock),
        _i2c(i2c),
        _output_km_h(output_km_h),
        _read_timer(clock, config.read_period_ms) {}

    void tick()
    {
        if(_read_timer.poll())
        {
            uint8_t buffer[4];
            uint32_t n_bytes_read;
            if(_i2c->read(buffer, sizeof(buffer), n_bytes_read))
            {
                uint8_t pressure_status = (buffer[0] >> 6);
                if(pressure_status != 0)
                {
                    // Invalid or old data
                    return;
                }

                uint16_t count = ((buffer[0] & 0b00111111) << 8) + buffer[1];
                if(count < COUNT_MIN || count > COUNT_MAX)
                {
                    // Invalid data
                    return;
                }

                float delta_pressure_pa = (count - COUNT_HALF) * PRESSURE_MAX_PA / COUNT_HALF;
                if(delta_pressure_pa <= 0)
                {
                    _output_km_h->update(0.0f);
                }
                else
                {
                    float airspeed_m_s = sqrt(2 * delta_pressure_pa / RHO_KG_M3);
                    _output_km_h->update(airspeed_m_s);
                }
            }       
        } 
    }

    dragonfly_msgs::enums::DiagLevel getDiagLevel()
    {
        return _output_km_h->getState().is_ok ? dragonfly_msgs::enums::DiagLevel::OK : dragonfly_msgs::enums::DiagLevel::ERR;
    }

private:

    static constexpr float COUNT_MIN = 0.1 * 16383;
    static constexpr float COUNT_MAX = 0.9 * 16383;
    static constexpr float COUNT_HALF = (COUNT_MIN + COUNT_MAX) / 2;
    static constexpr float PRESSURE_MAX_PA = 6894.76;
    static constexpr float RHO_KG_M3 = 1.225;
    ClockInterface* _clock;
    IoInterface* _i2c;
    Periodic<float>* _output_km_h;
    Timer _read_timer;
};