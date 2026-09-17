
#pragma once

#include <dragonfly_msgs/internal/Common.h>

#include <dragonfly_msgs/enums/ControlModes.h>
#include <dragonfly_msgs/msgs/flight_controller/Actuators.h>
#include <dragonfly_msgs/msgs/flight_controller/Navigation.h>
#include <dragonfly_msgs/msgs/flight_controller/InputDiagsOverview.h>


namespace dragonfly_msgs { 
namespace msgs { 
namespace flight_controller { 


class StateOverview
{
public:

        base_types::Enum<enums::ControlModes> control_mode;
        Actuators actuators;
        Navigation navigation;
        InputDiagsOverview input_diags;
        base_types::Int<0,100> battery_level_percent;


    static constexpr uint32_t _SerialisationSizeBits = 
        base_types::Enum<enums::ControlModes>::_SerialisationSizeBits + 
        Actuators::_SerialisationSizeBits + 
        Navigation::_SerialisationSizeBits + 
        InputDiagsOverview::_SerialisationSizeBits + 
        base_types::Int<0,100>::_SerialisationSizeBits + 
        0;
    static constexpr uint32_t _Id = 4;
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
        control_mode._serialise(buffer);
        actuators._serialise(buffer);
        navigation._serialise(buffer);
        input_diags._serialise(buffer);
        battery_level_percent._serialise(buffer);
    }

    void _deserialise(serialisation::RBitBuffer& buffer)
    {
        control_mode._deserialise(buffer);
        actuators._deserialise(buffer);
        navigation._deserialise(buffer);
        input_diags._deserialise(buffer);
        battery_level_percent._deserialise(buffer);
    }

    nlohmann::json _serialiseJson() const
    {
        nlohmann::json res;
        res["control_mode"] = control_mode._serialiseJson();
        res["actuators"] = actuators._serialiseJson();
        res["navigation"] = navigation._serialiseJson();
        res["input_diags"] = input_diags._serialiseJson();
        res["battery_level_percent"] = battery_level_percent._serialiseJson();
        return res;
    }

};

}}}

