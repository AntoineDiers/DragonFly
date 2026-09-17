#pragma once

#include "../Serialisation.h"

#include <algorithm>

namespace dragonfly_msgs::base_types
{

    template<uint8_t NBits>
    class BaseType
    {
    public:

        static constexpr uint32_t _SerialisationSizeBits = NBits;

        void _serialise(serialisation::WBitBuffer& buffer) const
        {
            buffer.write(_val, NBits);
        }
        void _deserialise(serialisation::RBitBuffer& buffer)
        {
            _val = buffer.read(NBits);
        }

    protected:

        static constexpr uint64_t _MaxVal = (1 << NBits) - 1;

        typedef
            std::conditional_t<(NBits <= 8),  uint8_t,
            std::conditional_t<(NBits <= 16), uint16_t,
            std::conditional_t<(NBits <= 32), uint32_t,
            uint64_t>>> _StorageType;

        _StorageType _val = 0;
    };

    constexpr uint8_t neededBits(uint64_t max_val)
    {
        uint8_t bits = 0;

        while (max_val > 0)
        {
            ++bits;
            max_val >>= 1;
        }

        return bits;
    }

};