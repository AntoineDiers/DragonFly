
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
    static constexpr uint8_t _Id = 7;

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

    void _print(uint32_t n_tabs = 0) const
    {
        char tabs[4 * 5 + 1] = {0};
        for(uint32_t i = 0; i < std::min(4 * n_tabs, (uint32_t)sizeof(tabs) - 1); i++)
        {
            tabs[i] = ' ';
        }

        std::cout << "{\n";
        internal::printField<link_stats::LinkStats>("gs", gs, n_tabs + 1);
        internal::printField<link_stats::LinkStats>("fc_via_rf", fc_via_rf, n_tabs + 1);
        internal::printField<link_stats::LinkStats>("fc_via_4g", fc_via_4g, n_tabs + 1);
        internal::printField<link_stats::LinkStats>("pc_via_rf", pc_via_rf, n_tabs + 1);
        internal::printField<link_stats::LinkStats>("pc_via_4g", pc_via_4g, n_tabs + 1);

        std::cout << tabs << "}";
    }
};

}}}

