
#pragma once

#include <dragonfly_msgs/Utils.h>



namespace dragonfly_msgs { 
namespace msg { 
namespace link_stats { 


class LinkStats
{
public:

    uint16_t ping_ms;
    uint8_t packet_loss_percent;


    static constexpr uint32_t _BodySize = sizeof(uint16_t) + sizeof(uint8_t) +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 11;

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
        internal::deserialiseField<uint16_t>(ping_ms, buffer);
        internal::deserialiseField<uint8_t>(packet_loss_percent, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<uint16_t>(ping_ms, buffer);
        internal::serialiseField<uint8_t>(packet_loss_percent, buffer);

    }

    friend std::ostream& operator<<(std::ostream& os, const LinkStats& msg)
    {

        os << "{ ";
        internal::printField<uint16_t>(os, "ping_ms", msg.ping_ms);
        internal::printField<uint8_t>(os, "packet_loss_percent", msg.packet_loss_percent);

        os << " }";
        return os;
    }
};

}}}

