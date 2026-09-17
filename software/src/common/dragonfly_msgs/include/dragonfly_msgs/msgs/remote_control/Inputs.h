
#pragma once

#include <dragonfly_msgs/internal/Common.h>



namespace dragonfly_msgs { 
namespace msgs { 
namespace remote_control { 


class Inputs
{
public:

        base_types::Boolean arm_enabled;
        base_types::Boolean auto_enabled;
        base_types::Float<0,100,8> throttle_percent;
        base_types::Float<-1,1,8> left_right;
        base_types::Float<-1,1,8> down_up;
        base_types::Int<0,45> flaps_deg;
        base_types::Int<0,100> roll_stab_level_percent;
        base_types::Int<0,100> pitch_stab_level_percent;


    static constexpr uint32_t _SerialisationSizeBits = 
        base_types::Boolean::_SerialisationSizeBits + 
        base_types::Boolean::_SerialisationSizeBits + 
        base_types::Float<0,100,8>::_SerialisationSizeBits + 
        base_types::Float<-1,1,8>::_SerialisationSizeBits + 
        base_types::Float<-1,1,8>::_SerialisationSizeBits + 
        base_types::Int<0,45>::_SerialisationSizeBits + 
        base_types::Int<0,100>::_SerialisationSizeBits + 
        base_types::Int<0,100>::_SerialisationSizeBits + 
        0;
    static constexpr uint32_t _Id = 8;
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
        arm_enabled._serialise(buffer);
        auto_enabled._serialise(buffer);
        throttle_percent._serialise(buffer);
        left_right._serialise(buffer);
        down_up._serialise(buffer);
        flaps_deg._serialise(buffer);
        roll_stab_level_percent._serialise(buffer);
        pitch_stab_level_percent._serialise(buffer);
    }

    void _deserialise(serialisation::RBitBuffer& buffer)
    {
        arm_enabled._deserialise(buffer);
        auto_enabled._deserialise(buffer);
        throttle_percent._deserialise(buffer);
        left_right._deserialise(buffer);
        down_up._deserialise(buffer);
        flaps_deg._deserialise(buffer);
        roll_stab_level_percent._deserialise(buffer);
        pitch_stab_level_percent._deserialise(buffer);
    }

    nlohmann::json _serialiseJson() const
    {
        nlohmann::json res;
        res["arm_enabled"] = arm_enabled._serialiseJson();
        res["auto_enabled"] = auto_enabled._serialiseJson();
        res["throttle_percent"] = throttle_percent._serialiseJson();
        res["left_right"] = left_right._serialiseJson();
        res["down_up"] = down_up._serialiseJson();
        res["flaps_deg"] = flaps_deg._serialiseJson();
        res["roll_stab_level_percent"] = roll_stab_level_percent._serialiseJson();
        res["pitch_stab_level_percent"] = pitch_stab_level_percent._serialiseJson();
        return res;
    }

};

}}}

