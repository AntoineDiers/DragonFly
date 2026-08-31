#pragma once

#include <ads1115_driver/Ads1115Driver.h>

class Ads1115Mock : public IoInterface
{
public:

    Ads1115Mock(ClockInterface* clock) :
        _clock(clock) {}

    void setPinVoltage(const Ads1115Driver::Pin& pin, const std::optional<float> voltage)
    {
        switch (pin)
        {
            case Ads1115Driver::Pin::A0 : { _a0_voltage = voltage; break; }
            case Ads1115Driver::Pin::A1 : { _a1_voltage = voltage; break; }
            case Ads1115Driver::Pin::A2 : { _a2_voltage = voltage; break; }
            case Ads1115Driver::Pin::A3 : { _a3_voltage = voltage; break; }
        }
    }

    virtual bool read(uint8_t *data, uint32_t n_bytes_to_read, uint32_t& n_bytes_read) override
    {
        bool res = false;
        if(_tx_buffer && n_bytes_to_read == _tx_buffer.value().size())
        {
            res = true;
            for(uint32_t i = 0; i < n_bytes_to_read; i++)
            {
                data[i] = _tx_buffer.value()[i];
            }
            n_bytes_read = n_bytes_to_read;
        }
        _tx_buffer = std::nullopt;
        return res;
    }

    virtual bool write(const uint8_t *data, uint32_t n_bytes_to_write, uint32_t& n_bytes_written) override
    {
        uint32_t time_since_reading_start = (_clock->getTime_us() - _pending_reading->read_start_stamp) / 1000;

        if(n_bytes_to_write == 1 && data[0] == 0x00 && _pending_reading && time_since_reading_start > Ads1115Driver::spsToMs(_pending_reading->sps))
        {
            std::optional<float> voltage;
            switch (_pending_reading->pin)
            {
                case Ads1115Driver::Pin::A0 : { voltage = _a0_voltage; break; }
                case Ads1115Driver::Pin::A1 : { voltage = _a1_voltage; break; }
                case Ads1115Driver::Pin::A2 : { voltage = _a2_voltage; break; }
                case Ads1115Driver::Pin::A3 : { voltage = _a3_voltage; break; }
            }

            if(voltage)
            {
                int16_t raw = voltage.value() * 32768.0f / 4.096f;
                _tx_buffer.emplace();
                _tx_buffer.value()[0] = raw >> 8 & 0xFF;
                _tx_buffer.value()[1] = raw & 0xFF;

                n_bytes_written = n_bytes_to_write;
                return true;
            }
            return false;
        }

        if(n_bytes_to_write == 1 && data[0] == 0x01 && _pending_reading)
        {
            bool reading_ready = time_since_reading_start > Ads1115Driver::spsToMs(_pending_reading->sps);

            _tx_buffer.emplace();
            _tx_buffer.value()[0] = reading_ready ? 0b10000000 : 0;
            _tx_buffer.value()[1] = 0;

            n_bytes_written = n_bytes_to_write;
            return true;
        }

        if( n_bytes_to_write == 3 
            && data[0] == 0x01
            && (data[1] & 0b11001111) == 0xC3
            && (data[1] & 0b00011111) == 0x03)
        {
            Ads1115Driver::Pin pin = (Ads1115Driver::Pin)((data[1] >> 4) & 0b11);
            Ads1115Driver::SPS sps = (Ads1115Driver::SPS)((data[2] >> 5) & 0b111);

            _pending_reading = Ads1115Driver::PendingReadingInfo
            {
                .pin = pin,
                .sps = sps,
                .read_start_stamp = _clock->getTime_us()
            };

            n_bytes_written = n_bytes_to_write;
            return true;
        }

        return false;
    }   

private:

    ClockInterface* _clock;
    std::optional<Ads1115Driver::PendingReadingInfo> _pending_reading;

    std::optional<std::array<uint8_t, 2>> _tx_buffer;

    std::optional<float> _a0_voltage;
    std::optional<float> _a1_voltage;
    std::optional<float> _a2_voltage;
    std::optional<float> _a3_voltage;
};