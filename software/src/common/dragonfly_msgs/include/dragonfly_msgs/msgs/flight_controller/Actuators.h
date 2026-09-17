
#pragma once

#include <dragonfly_msgs/internal/Common.h>



namespace dragonfly_msgs { 
namespace msgs { 
namespace flight_controller { 


class Actuators
{
public:

        base_types::Int<0,100> throttle_percent;
        base_types::Int<-45,45> left_flaperon_deg;
        base_types::Int<-45,45> right_flaperon_deg;
        base_types::Int<-45,45> left_ruddervator_deg;
        base_types::Int<-45,45> right_ruddervator_deg;


    static constexpr uint32_t _SerialisationSizeBits = 
        base_types::Int<0,100>::_SerialisationSizeBits + 
        base_types::Int<-45,45>::_SerialisationSizeBits + 
        base_types::Int<-45,45>::_SerialisationSizeBits + 
        base_types::Int<-45,45>::_SerialisationSizeBits + 
        base_types::Int<-45,45>::_SerialisationSizeBits + 
        0;
    static constexpr uint32_t _Id = 0;
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
        throttle_percent._serialise(buffer);
        left_flaperon_deg._serialise(buffer);
        right_flaperon_deg._serialise(buffer);
        left_ruddervator_deg._serialise(buffer);
        right_ruddervator_deg._serialise(buffer);
    }

    void _deserialise(serialisation::RBitBuffer& buffer)
    {
        throttle_percent._deserialise(buffer);
        left_flaperon_deg._deserialise(buffer);
        right_flaperon_deg._deserialise(buffer);
        left_ruddervator_deg._deserialise(buffer);
        right_ruddervator_deg._deserialise(buffer);
    }

    nlohmann::json _serialiseJson() const
    {
        nlohmann::json res;
        res["throttle_percent"] = throttle_percent._serialiseJson();
        res["left_flaperon_deg"] = left_flaperon_deg._serialiseJson();
        res["right_flaperon_deg"] = right_flaperon_deg._serialiseJson();
        res["left_ruddervator_deg"] = left_ruddervator_deg._serialiseJson();
        res["right_ruddervator_deg"] = right_ruddervator_deg._serialiseJson();
        return res;
    }

};

}}}

