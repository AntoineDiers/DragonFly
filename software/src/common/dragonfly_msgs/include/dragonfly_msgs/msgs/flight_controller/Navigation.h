
#pragma once

#include <dragonfly_msgs/internal/Common.h>



namespace dragonfly_msgs { 
namespace msgs { 
namespace flight_controller { 


class Navigation
{
public:

        base_types::Float<-90,90,9> roll_deg;
        base_types::Float<-90,90,9> pitch_deg;
        base_types::Float<0,150,10> airspeed_km_h;
        base_types::Float<0,1000,12> altitude_m;


    static constexpr uint32_t _SerialisationSizeBits = 
        base_types::Float<-90,90,9>::_SerialisationSizeBits + 
        base_types::Float<-90,90,9>::_SerialisationSizeBits + 
        base_types::Float<0,150,10>::_SerialisationSizeBits + 
        base_types::Float<0,1000,12>::_SerialisationSizeBits + 
        0;
    static constexpr uint32_t _Id = 3;
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
        roll_deg._serialise(buffer);
        pitch_deg._serialise(buffer);
        airspeed_km_h._serialise(buffer);
        altitude_m._serialise(buffer);
    }

    void _deserialise(serialisation::RBitBuffer& buffer)
    {
        roll_deg._deserialise(buffer);
        pitch_deg._deserialise(buffer);
        airspeed_km_h._deserialise(buffer);
        altitude_m._deserialise(buffer);
    }

    nlohmann::json _serialiseJson() const
    {
        nlohmann::json res;
        res["roll_deg"] = roll_deg._serialiseJson();
        res["pitch_deg"] = pitch_deg._serialiseJson();
        res["airspeed_km_h"] = airspeed_km_h._serialiseJson();
        res["altitude_m"] = altitude_m._serialiseJson();
        return res;
    }

};

}}}

