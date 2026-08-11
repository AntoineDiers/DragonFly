
#pragma once

#include <dragonfly_msgs/Utils.h>



namespace dragonfly_msgs { 
namespace msg { 


class PeriodicDataState
{
public:

    uint8_t state;
    uint32_t time_since_last_ms;
    float rate_hz;


    static constexpr uint32_t _BodySize = sizeof(uint8_t) + sizeof(uint32_t) + sizeof(float) +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 2;

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
        internal::deserialiseField<uint8_t>(state, buffer);
        internal::deserialiseField<uint32_t>(time_since_last_ms, buffer);
        internal::deserialiseField<float>(rate_hz, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<uint8_t>(state, buffer);
        internal::serialiseField<uint32_t>(time_since_last_ms, buffer);
        internal::serialiseField<float>(rate_hz, buffer);

    }

    friend std::ostream& operator<<(std::ostream& os, const PeriodicDataState& msg)
    {

        os << "{ ";
        internal::printField<uint8_t>(os, "state", msg.state);
        internal::printField<uint32_t>(os, "time_since_last_ms", msg.time_since_last_ms);
        internal::printField<float>(os, "rate_hz", msg.rate_hz);

        os << " }";
        return os;
    }
};

}}

