#pragma once

#include <nlohmann/json.hpp>

namespace dragonfly_msgs::serialisation
{
    constexpr uint32_t sizeBitsToBytes(uint32_t size_bits)
    {
        return (size_bits / 8) + (size_bits % 8 > 0 ? 1 : 0);
    }

    class RBitBuffer
    {
    public:    
    
        RBitBuffer(const uint8_t* data) : 
            _data(data) {}

        uint64_t read(uint8_t n_bits)
        {
            uint64_t res = 0;
            for(int i = 0; i < n_bits; i++)
            {
                res += (uint64_t)(readBit() ? 1 : 0) << i;
            }
            return res;
        }

    private:

        bool readBit()
        {
            bool res = (_data[_pos_bit / 8] & (0b10000000 >> (_pos_bit % 8))) > 0;
            _pos_bit++;
            return res;
        }

        const uint8_t* _data;
        uint32_t _pos_bit = 0;
    };

    class WBitBuffer
    {
    public:

        WBitBuffer(uint8_t* data) : 
            _data(data) {}

        void write(uint64_t val, uint8_t n_bits)
        {
            for(int i = 0; i < n_bits; i++)
            {
                writeBit(val & ((uint64_t)1 << i));
            }
        }

    private:

        void writeBit(bool val)
        {
            _data[_pos_bit / 8] &= ~(0b10000000 >> (_pos_bit % 8));
            _data[_pos_bit / 8] |= (val ? 0b10000000 : 0x00) >> (_pos_bit % 8);
            _pos_bit++;
        }

        uint8_t* _data;
        uint32_t _pos_bit = 0;
    };
}
        
        