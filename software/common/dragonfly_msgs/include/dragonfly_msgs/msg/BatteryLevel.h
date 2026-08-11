
#pragma once

#include <dragonfly_msgs/Utils.h>



namespace dragonfly_msgs { 
namespace msg { 


class BatteryLevel
{
public:

    float battery_level_percent;


    static constexpr uint32_t _BodySize = sizeof(float) +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 1;

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
    }

    void _deserialiseFields(const uint8_t* &buffer)
    {
        internal::deserialiseField<float>(battery_level_percent, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<float>(battery_level_percent, buffer);

    }

    friend std::ostream& operator<<(std::ostream& os, const BatteryLevel& msg)
    {

        os << "{ ";
        internal::printField<float>(os, "battery_level_percent", msg.battery_level_percent);

        os << " }";
        return os;
    }
};

}}

