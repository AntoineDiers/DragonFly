
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
    float roll_stabilization_level_percent;
    float pitch_stabilization_level_percent;
    float flaps_angle_deg;
    float throttle_percent;
    float left_right;
    float down_up;


    static constexpr uint32_t _BodySize = sizeof(bool) + sizeof(bool) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 7;

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
    }

    void _deserialiseFields(const uint8_t* &buffer)
    {
        internal::deserialiseField<bool>(is_armed, buffer);
        internal::deserialiseField<bool>(auto_mode, buffer);
        internal::deserialiseField<float>(roll_stabilization_level_percent, buffer);
        internal::deserialiseField<float>(pitch_stabilization_level_percent, buffer);
        internal::deserialiseField<float>(flaps_angle_deg, buffer);
        internal::deserialiseField<float>(throttle_percent, buffer);
        internal::deserialiseField<float>(left_right, buffer);
        internal::deserialiseField<float>(down_up, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<bool>(is_armed, buffer);
        internal::serialiseField<bool>(auto_mode, buffer);
        internal::serialiseField<float>(roll_stabilization_level_percent, buffer);
        internal::serialiseField<float>(pitch_stabilization_level_percent, buffer);
        internal::serialiseField<float>(flaps_angle_deg, buffer);
        internal::serialiseField<float>(throttle_percent, buffer);
        internal::serialiseField<float>(left_right, buffer);
        internal::serialiseField<float>(down_up, buffer);

    }

    friend std::ostream& operator<<(std::ostream& os, const Inputs& msg)
    {

        os << "{ ";
        internal::printField<bool>(os, "is_armed", msg.is_armed);
        internal::printField<bool>(os, "auto_mode", msg.auto_mode);
        internal::printField<float>(os, "roll_stabilization_level_percent", msg.roll_stabilization_level_percent);
        internal::printField<float>(os, "pitch_stabilization_level_percent", msg.pitch_stabilization_level_percent);
        internal::printField<float>(os, "flaps_angle_deg", msg.flaps_angle_deg);
        internal::printField<float>(os, "throttle_percent", msg.throttle_percent);
        internal::printField<float>(os, "left_right", msg.left_right);
        internal::printField<float>(os, "down_up", msg.down_up);

        os << " }";
        return os;
    }
};

}}}

