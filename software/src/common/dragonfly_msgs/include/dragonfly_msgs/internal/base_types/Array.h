#pragma once

#include "../Serialisation.h"

namespace dragonfly_msgs::base_types
{
    template<typename T, uint32_t Size>
    class Array
    {
    public:

        static constexpr uint32_t _SerialisationSizeBits = Size * T::_SerialisationSizeBits;

        T _data[Size];

        void _serialise(serialisation::WBitBuffer& buffer) const
        {
            for(uint32_t i = 0; i < Size; i++)
            {
                _data[i]._serialise(buffer);
            }
        }

        void _deserialise(serialisation::RBitBuffer& buffer)
        {
            for(uint32_t i = 0; i < Size; i++)
            {
                _data[i]._deserialise(buffer);
            }
        }

        nlohmann::json _serialiseJson() const
        {
            nlohmann::json res;
            for(uint32_t i = 0; i < Size; i++)
            {
                res.push_back(_data[i]._serialiseJson());
            }
            return res;
        }

        T& operator[](uint32_t i)
        {
            return _data[i];
        }
    };
}