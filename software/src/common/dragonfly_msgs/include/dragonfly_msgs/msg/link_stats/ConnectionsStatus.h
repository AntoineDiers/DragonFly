
#pragma once

#include <dragonfly_msgs/Utils.h>

#include <dragonfly_msgs/msg/link_stats/ConnectionStatus.h>
#include <dragonfly_msgs/msg/link_stats/ConnectionStatus.h>
#include <dragonfly_msgs/msg/link_stats/ConnectionStatus.h>
#include <dragonfly_msgs/msg/link_stats/ConnectionStatus.h>
#include <dragonfly_msgs/msg/link_stats/ConnectionStatus.h>


namespace dragonfly_msgs { 
namespace msg { 
namespace link_stats { 


class ConnectionsStatus
{
public:

    link_stats::ConnectionStatus local_peer_via_local_link;
    link_stats::ConnectionStatus direct_remote_peer_via_local_link;
    link_stats::ConnectionStatus indirect_remote_peer_via_local_link;
    link_stats::ConnectionStatus direct_remote_peer_via_remote_link;
    link_stats::ConnectionStatus indirect_remote_peer_via_remote_link;


    static constexpr uint32_t _BodySize = link_stats::ConnectionStatus::_BodySize + link_stats::ConnectionStatus::_BodySize + link_stats::ConnectionStatus::_BodySize + link_stats::ConnectionStatus::_BodySize + link_stats::ConnectionStatus::_BodySize +  0;
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
        buffer.size = _SerialisationSize;
    }

    void _deserialiseFields(const uint8_t* &buffer)
    {
        internal::deserialiseField<link_stats::ConnectionStatus>(local_peer_via_local_link, buffer);
        internal::deserialiseField<link_stats::ConnectionStatus>(direct_remote_peer_via_local_link, buffer);
        internal::deserialiseField<link_stats::ConnectionStatus>(indirect_remote_peer_via_local_link, buffer);
        internal::deserialiseField<link_stats::ConnectionStatus>(direct_remote_peer_via_remote_link, buffer);
        internal::deserialiseField<link_stats::ConnectionStatus>(indirect_remote_peer_via_remote_link, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<link_stats::ConnectionStatus>(local_peer_via_local_link, buffer);
        internal::serialiseField<link_stats::ConnectionStatus>(direct_remote_peer_via_local_link, buffer);
        internal::serialiseField<link_stats::ConnectionStatus>(indirect_remote_peer_via_local_link, buffer);
        internal::serialiseField<link_stats::ConnectionStatus>(direct_remote_peer_via_remote_link, buffer);
        internal::serialiseField<link_stats::ConnectionStatus>(indirect_remote_peer_via_remote_link, buffer);

    }

    void _print(uint32_t n_tabs = 0) const
    {
        char tabs[4 * 5 + 1] = {0};
        for(uint32_t i = 0; i < std::min(4 * n_tabs, (uint32_t)sizeof(tabs) - 1); i++)
        {
            tabs[i] = ' ';
        }

        std::cout << "{\n";
        internal::printField<link_stats::ConnectionStatus>("local_peer_via_local_link", local_peer_via_local_link, n_tabs + 1);
        internal::printField<link_stats::ConnectionStatus>("direct_remote_peer_via_local_link", direct_remote_peer_via_local_link, n_tabs + 1);
        internal::printField<link_stats::ConnectionStatus>("indirect_remote_peer_via_local_link", indirect_remote_peer_via_local_link, n_tabs + 1);
        internal::printField<link_stats::ConnectionStatus>("direct_remote_peer_via_remote_link", direct_remote_peer_via_remote_link, n_tabs + 1);
        internal::printField<link_stats::ConnectionStatus>("indirect_remote_peer_via_remote_link", indirect_remote_peer_via_remote_link, n_tabs + 1);

        std::cout << tabs << "}";
    }
};

}}}

