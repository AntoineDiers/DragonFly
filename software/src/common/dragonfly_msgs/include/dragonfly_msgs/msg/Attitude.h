
#pragma once

#include <dragonfly_msgs/Utils.h>



namespace dragonfly_msgs { 
namespace msg { 


class Attitude
{
public:

    float roll_deg;
    float pitch_deg;


    static constexpr uint32_t _BodySize = sizeof(float) + sizeof(float) +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 16;

    template<uint32_t BufferSize>
    bool _deserialise(const Buffer<BufferSize>& buffer)
    {
        static_assert(BufferSize >= _SerialisationSize);

        const uint8_t* ptr = buffer.data.data();

        if(buffer.size != _SerialisationSize) { return false; }
        MsgId id;
        internal::deserialiseField(id, ptr);
        if(id != _Id) { return false; }

        _deserialiseFields(ptr);

        return true;
    }
    
    template<uint32_t BufferSize>
    void _serialise(Buffer<BufferSize>& buffer) const
    {
        static_assert(BufferSize >= _SerialisationSize);

        uint8_t* ptr = buffer.data.data();
        internal::serialiseField(_Id, ptr);
        _serialiseFields(ptr);
        buffer.size = _SerialisationSize;
    }

    void _deserialiseFields(const uint8_t* &buffer)
    {
        internal::deserialiseField<float>(roll_deg, buffer);
        internal::deserialiseField<float>(pitch_deg, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<float>(roll_deg, buffer);
        internal::serialiseField<float>(pitch_deg, buffer);

    }

    void _print(uint32_t n_tabs = 0) const
    {
        char tabs[4 * 5 + 1] = {0};
        for(uint32_t i = 0; i < std::min(4 * n_tabs, (uint32_t)sizeof(tabs) - 1); i++)
        {
            tabs[i] = ' ';
        }

        std::cout << "{\n";
        internal::printField<float>("roll_deg", roll_deg, n_tabs + 1);
        internal::printField<float>("pitch_deg", pitch_deg, n_tabs + 1);

        std::cout << tabs << "}";
    }
};

}}

