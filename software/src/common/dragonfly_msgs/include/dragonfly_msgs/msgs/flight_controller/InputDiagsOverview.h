
#pragma once

#include <dragonfly_msgs/internal/Common.h>

#include <dragonfly_msgs/enums/DiagLevel.h>


namespace dragonfly_msgs { 
namespace msgs { 
namespace flight_controller { 


class InputDiagsOverview
{
public:

        base_types::Enum<enums::DiagLevel> remote_control;
        base_types::Enum<enums::DiagLevel> airspeed;
        base_types::Enum<enums::DiagLevel> attitude;
        base_types::Enum<enums::DiagLevel> altitude;
        base_types::Enum<enums::DiagLevel> gps;
        base_types::Enum<enums::DiagLevel> battery_level;


    static constexpr uint32_t _SerialisationSizeBits = 
        base_types::Enum<enums::DiagLevel>::_SerialisationSizeBits + 
        base_types::Enum<enums::DiagLevel>::_SerialisationSizeBits + 
        base_types::Enum<enums::DiagLevel>::_SerialisationSizeBits + 
        base_types::Enum<enums::DiagLevel>::_SerialisationSizeBits + 
        base_types::Enum<enums::DiagLevel>::_SerialisationSizeBits + 
        base_types::Enum<enums::DiagLevel>::_SerialisationSizeBits + 
        0;
    static constexpr uint32_t _Id = 2;
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
        remote_control._serialise(buffer);
        airspeed._serialise(buffer);
        attitude._serialise(buffer);
        altitude._serialise(buffer);
        gps._serialise(buffer);
        battery_level._serialise(buffer);
    }

    void _deserialise(serialisation::RBitBuffer& buffer)
    {
        remote_control._deserialise(buffer);
        airspeed._deserialise(buffer);
        attitude._deserialise(buffer);
        altitude._deserialise(buffer);
        gps._deserialise(buffer);
        battery_level._deserialise(buffer);
    }

    nlohmann::json _serialiseJson() const
    {
        nlohmann::json res;
        res["remote_control"] = remote_control._serialiseJson();
        res["airspeed"] = airspeed._serialiseJson();
        res["attitude"] = attitude._serialiseJson();
        res["altitude"] = altitude._serialiseJson();
        res["gps"] = gps._serialiseJson();
        res["battery_level"] = battery_level._serialiseJson();
        return res;
    }

};

}}}

