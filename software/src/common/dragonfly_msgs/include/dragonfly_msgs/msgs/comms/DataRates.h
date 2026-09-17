
#pragma once

#include <dragonfly_msgs/internal/Common.h>



namespace dragonfly_msgs { 
namespace msgs { 
namespace comms { 


class DataRates
{
public:

        base_types::Float<0,1000,16> local_link_rx_kbps;
        base_types::Float<0,1000,16> local_link_tx_kbps;
        base_types::Float<0,1000,16> remote_link_rx_kbps;
        base_types::Float<0,1000,16> remote_link_tx_kbps;


    static constexpr uint32_t _SerialisationSizeBits = 
        base_types::Float<0,1000,16>::_SerialisationSizeBits + 
        base_types::Float<0,1000,16>::_SerialisationSizeBits + 
        base_types::Float<0,1000,16>::_SerialisationSizeBits + 
        base_types::Float<0,1000,16>::_SerialisationSizeBits + 
        0;
    static constexpr uint32_t _Id = 5;
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
        local_link_rx_kbps._serialise(buffer);
        local_link_tx_kbps._serialise(buffer);
        remote_link_rx_kbps._serialise(buffer);
        remote_link_tx_kbps._serialise(buffer);
    }

    void _deserialise(serialisation::RBitBuffer& buffer)
    {
        local_link_rx_kbps._deserialise(buffer);
        local_link_tx_kbps._deserialise(buffer);
        remote_link_rx_kbps._deserialise(buffer);
        remote_link_tx_kbps._deserialise(buffer);
    }

    nlohmann::json _serialiseJson() const
    {
        nlohmann::json res;
        res["local_link_rx_kbps"] = local_link_rx_kbps._serialiseJson();
        res["local_link_tx_kbps"] = local_link_tx_kbps._serialiseJson();
        res["remote_link_rx_kbps"] = remote_link_rx_kbps._serialiseJson();
        res["remote_link_tx_kbps"] = remote_link_tx_kbps._serialiseJson();
        return res;
    }

};

}}}

