
#pragma once

#include <dragonfly_msgs/Utils.h>



namespace dragonfly_msgs { 
namespace msg { 
namespace remote_control { 


class Inputs
{
public:

    bool is_armed;
    bool auto_mode;
    uint8_t roll_stabilization_level;
    uint8_t pitch_stabilization_level;
    uint8_t flaps;
    uint8_t throttle;
    uint8_t left_right;
    uint8_t down_up;


    static constexpr uint32_t _BodySize = sizeof(bool) + sizeof(bool) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 8;

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
        internal::deserialiseField<bool>(is_armed, buffer);
        internal::deserialiseField<bool>(auto_mode, buffer);
        internal::deserialiseField<uint8_t>(roll_stabilization_level, buffer);
        internal::deserialiseField<uint8_t>(pitch_stabilization_level, buffer);
        internal::deserialiseField<uint8_t>(flaps, buffer);
        internal::deserialiseField<uint8_t>(throttle, buffer);
        internal::deserialiseField<uint8_t>(left_right, buffer);
        internal::deserialiseField<uint8_t>(down_up, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<bool>(is_armed, buffer);
        internal::serialiseField<bool>(auto_mode, buffer);
        internal::serialiseField<uint8_t>(roll_stabilization_level, buffer);
        internal::serialiseField<uint8_t>(pitch_stabilization_level, buffer);
        internal::serialiseField<uint8_t>(flaps, buffer);
        internal::serialiseField<uint8_t>(throttle, buffer);
        internal::serialiseField<uint8_t>(left_right, buffer);
        internal::serialiseField<uint8_t>(down_up, buffer);

    }

    void _print(uint32_t n_tabs = 0) const
    {
        char tabs[4 * 5 + 1] = {0};
        for(uint32_t i = 0; i < std::min(4 * n_tabs, (uint32_t)sizeof(tabs) - 1); i++)
        {
            tabs[i] = ' ';
        }

        std::cout << "{\n";
        internal::printField<bool>("is_armed", is_armed, n_tabs + 1);
        internal::printField<bool>("auto_mode", auto_mode, n_tabs + 1);
        internal::printField<uint8_t>("roll_stabilization_level", roll_stabilization_level, n_tabs + 1);
        internal::printField<uint8_t>("pitch_stabilization_level", pitch_stabilization_level, n_tabs + 1);
        internal::printField<uint8_t>("flaps", flaps, n_tabs + 1);
        internal::printField<uint8_t>("throttle", throttle, n_tabs + 1);
        internal::printField<uint8_t>("left_right", left_right, n_tabs + 1);
        internal::printField<uint8_t>("down_up", down_up, n_tabs + 1);

        std::cout << tabs << "}";
    }
};

}}}

