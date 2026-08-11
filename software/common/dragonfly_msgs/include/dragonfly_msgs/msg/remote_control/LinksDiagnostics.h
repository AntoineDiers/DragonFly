
#pragma once

#include <dragonfly_msgs/Utils.h>

#include <dragonfly_msgs/msg/link_stats/LinkStats.h>
#include <dragonfly_msgs/msg/link_stats/LinkStats.h>
#include <dragonfly_msgs/msg/link_stats/LinkStats.h>
#include <dragonfly_msgs/msg/link_stats/LinkStats.h>
#include <dragonfly_msgs/msg/link_stats/LinkStats.h>


namespace dragonfly_msgs { 
namespace msg { 
namespace remote_control { 


class LinksDiagnostics
{
public:

    link_stats::LinkStats gs;
    link_stats::LinkStats fc_via_rf;
    link_stats::LinkStats fc_via_4g;
    link_stats::LinkStats pc_via_rf;
    link_stats::LinkStats pc_via_4g;


    static constexpr uint32_t _BodySize = link_stats::LinkStats::_BodySize + link_stats::LinkStats::_BodySize + link_stats::LinkStats::_BodySize + link_stats::LinkStats::_BodySize + link_stats::LinkStats::_BodySize +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 6;

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
        internal::deserialiseField<link_stats::LinkStats>(gs, buffer);
        internal::deserialiseField<link_stats::LinkStats>(fc_via_rf, buffer);
        internal::deserialiseField<link_stats::LinkStats>(fc_via_4g, buffer);
        internal::deserialiseField<link_stats::LinkStats>(pc_via_rf, buffer);
        internal::deserialiseField<link_stats::LinkStats>(pc_via_4g, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<link_stats::LinkStats>(gs, buffer);
        internal::serialiseField<link_stats::LinkStats>(fc_via_rf, buffer);
        internal::serialiseField<link_stats::LinkStats>(fc_via_4g, buffer);
        internal::serialiseField<link_stats::LinkStats>(pc_via_rf, buffer);
        internal::serialiseField<link_stats::LinkStats>(pc_via_4g, buffer);

    }

    friend std::ostream& operator<<(std::ostream& os, const LinksDiagnostics& msg)
    {

        os << "{ ";
        internal::printField<link_stats::LinkStats>(os, "gs", msg.gs);
        internal::printField<link_stats::LinkStats>(os, "fc_via_rf", msg.fc_via_rf);
        internal::printField<link_stats::LinkStats>(os, "fc_via_4g", msg.fc_via_4g);
        internal::printField<link_stats::LinkStats>(os, "pc_via_rf", msg.pc_via_rf);
        internal::printField<link_stats::LinkStats>(os, "pc_via_4g", msg.pc_via_4g);

        os << " }";
        return os;
    }
};

}}}

