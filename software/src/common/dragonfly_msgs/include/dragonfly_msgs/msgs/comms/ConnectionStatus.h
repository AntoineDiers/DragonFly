
#pragma once

#include <dragonfly_msgs/internal/Common.h>



namespace dragonfly_msgs { 
namespace msgs { 
namespace comms { 


class ConnectionStatus
{
public:

        base_types::Boolean connected;
        base_types::Int<0,1000> ping_ms;
        base_types::Int<0,100> packet_loss_percent;


    static constexpr uint32_t _SerialisationSizeBits = 
        base_types::Boolean::_SerialisationSizeBits + 
        base_types::Int<0,1000>::_SerialisationSizeBits + 
        base_types::Int<0,100>::_SerialisationSizeBits + 
        0;
    static constexpr uint32_t _Id = 7;
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
        connected._serialise(buffer);
        ping_ms._serialise(buffer);
        packet_loss_percent._serialise(buffer);
    }

    void _deserialise(serialisation::RBitBuffer& buffer)
    {
        connected._deserialise(buffer);
        ping_ms._deserialise(buffer);
        packet_loss_percent._deserialise(buffer);
    }

    nlohmann::json _serialiseJson() const
    {
        nlohmann::json res;
        res["connected"] = connected._serialiseJson();
        res["ping_ms"] = ping_ms._serialiseJson();
        res["packet_loss_percent"] = packet_loss_percent._serialiseJson();
        return res;
    }

};

}}}

