
#pragma once

#include <dragonfly_msgs/Utils.h>

#include <dragonfly_msgs/msg/link_stats/LinkStats.h>
#include <dragonfly_msgs/msg/link_stats/LinkStats.h>
#include <dragonfly_msgs/msg/link_stats/LinkStats.h>
#include <dragonfly_msgs/msg/link_stats/LinkStats.h>
#include <dragonfly_msgs/msg/link_stats/LinkStats.h>


namespace dragonfly_msgs { 
namespace msg { 
namespace link_stats { 


class FlightControllerLinkStats
{
public:

    link_stats::LinkStats payload_computer;
    link_stats::LinkStats remote_control_via_rf;
    link_stats::LinkStats remote_control_via_4g;
    link_stats::LinkStats ground_station_via_rf;
    link_stats::LinkStats ground_station_via_4g;


    static constexpr uint32_t _BodySize = link_stats::LinkStats::_BodySize + link_stats::LinkStats::_BodySize + link_stats::LinkStats::_BodySize + link_stats::LinkStats::_BodySize + link_stats::LinkStats::_BodySize +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 12;

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
        internal::deserialiseField<link_stats::LinkStats>(payload_computer, buffer);
        internal::deserialiseField<link_stats::LinkStats>(remote_control_via_rf, buffer);
        internal::deserialiseField<link_stats::LinkStats>(remote_control_via_4g, buffer);
        internal::deserialiseField<link_stats::LinkStats>(ground_station_via_rf, buffer);
        internal::deserialiseField<link_stats::LinkStats>(ground_station_via_4g, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<link_stats::LinkStats>(payload_computer, buffer);
        internal::serialiseField<link_stats::LinkStats>(remote_control_via_rf, buffer);
        internal::serialiseField<link_stats::LinkStats>(remote_control_via_4g, buffer);
        internal::serialiseField<link_stats::LinkStats>(ground_station_via_rf, buffer);
        internal::serialiseField<link_stats::LinkStats>(ground_station_via_4g, buffer);

    }

    void _print(uint32_t n_tabs = 0) const
    {
        char tabs[4 * 5 + 1] = {0};
        for(uint32_t i = 0; i < std::min(4 * n_tabs, (uint32_t)sizeof(tabs) - 1); i++)
        {
            tabs[i] = ' ';
        }

        std::cout << "{\n";
        internal::printField<link_stats::LinkStats>("payload_computer", payload_computer, n_tabs + 1);
        internal::printField<link_stats::LinkStats>("remote_control_via_rf", remote_control_via_rf, n_tabs + 1);
        internal::printField<link_stats::LinkStats>("remote_control_via_4g", remote_control_via_4g, n_tabs + 1);
        internal::printField<link_stats::LinkStats>("ground_station_via_rf", ground_station_via_rf, n_tabs + 1);
        internal::printField<link_stats::LinkStats>("ground_station_via_4g", ground_station_via_4g, n_tabs + 1);

        std::cout << tabs << "}";
    }
};

}}}

