
#pragma once

#include <dragonfly_msgs/internal/Common.h>

#include <dragonfly_msgs/msgs/comms/ConnectionStatus.h>


namespace dragonfly_msgs { 
namespace msgs { 
namespace comms { 


class ConnectionsStatus
{
public:

        ConnectionStatus local_peer_via_local_link;
        ConnectionStatus direct_remote_peer_via_local_link;
        ConnectionStatus direct_remote_peer_via_remote_link;
        ConnectionStatus indirect_remote_peer_via_local_link;
        ConnectionStatus indirect_remote_peer_via_remote_link;


    static constexpr uint32_t _SerialisationSizeBits = 
        ConnectionStatus::_SerialisationSizeBits + 
        ConnectionStatus::_SerialisationSizeBits + 
        ConnectionStatus::_SerialisationSizeBits + 
        ConnectionStatus::_SerialisationSizeBits + 
        ConnectionStatus::_SerialisationSizeBits + 
        0;
    static constexpr uint32_t _Id = 6;
    static constexpr uint32_t _IdSizeBits = 4;
    static constexpr uint32_t _SizeBytes = serialisation::sizeBitsToBytes(_SerialisationSizeBits + _IdSizeBits);

    template<uint32_t BufferSize>
    void _serialise(Buffer<BufferSize>& buffer) const
    {
        static_assert(BufferSize >= _SizeBytes);

        serialisation::WBitBuffer bit_buffer(buffer.data.data());
        bit_buffer.write(_Id, _IdSizeBits);
        _serialise(bit_buffer);

        buffer.size = _SizeBytes;

        return;
    }

    template<uint32_t BufferSize>
    bool _deserialise(const Buffer<BufferSize>& buffer)
    {
        static_assert(BufferSize >= _SizeBytes);

        if(buffer.size != _SizeBytes) { return false; }
        
        serialisation::RBitBuffer bit_buffer(buffer.data.data());
        if(bit_buffer.read(_IdSizeBits) != _Id) { return false; }

        _deserialise(bit_buffer);
        return true;
    }
    
    void _serialise(serialisation::WBitBuffer& buffer) const
    {
        local_peer_via_local_link._serialise(buffer);
        direct_remote_peer_via_local_link._serialise(buffer);
        direct_remote_peer_via_remote_link._serialise(buffer);
        indirect_remote_peer_via_local_link._serialise(buffer);
        indirect_remote_peer_via_remote_link._serialise(buffer);
    }

    void _deserialise(serialisation::RBitBuffer& buffer)
    {
        local_peer_via_local_link._deserialise(buffer);
        direct_remote_peer_via_local_link._deserialise(buffer);
        direct_remote_peer_via_remote_link._deserialise(buffer);
        indirect_remote_peer_via_local_link._deserialise(buffer);
        indirect_remote_peer_via_remote_link._deserialise(buffer);
    }

    nlohmann::json _serialiseJson() const
    {
        nlohmann::json res;
        res["local_peer_via_local_link"] = local_peer_via_local_link._serialiseJson();
        res["direct_remote_peer_via_local_link"] = direct_remote_peer_via_local_link._serialiseJson();
        res["direct_remote_peer_via_remote_link"] = direct_remote_peer_via_remote_link._serialiseJson();
        res["indirect_remote_peer_via_local_link"] = indirect_remote_peer_via_local_link._serialiseJson();
        res["indirect_remote_peer_via_remote_link"] = indirect_remote_peer_via_remote_link._serialiseJson();
        return res;
    }

};

}}}

