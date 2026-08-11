
#pragma once

#include <dragonfly_msgs/Utils.h>



namespace dragonfly_msgs { 
namespace msg { 


class ActuatorCommands
{
public:

    float throttle_percent;
    float flaps_deg;
    float ailerons_deg;
    float elevator_deg;
    float rudder_deg;


    static constexpr uint32_t _BodySize = sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 3;

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
        internal::deserialiseField<float>(throttle_percent, buffer);
        internal::deserialiseField<float>(flaps_deg, buffer);
        internal::deserialiseField<float>(ailerons_deg, buffer);
        internal::deserialiseField<float>(elevator_deg, buffer);
        internal::deserialiseField<float>(rudder_deg, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<float>(throttle_percent, buffer);
        internal::serialiseField<float>(flaps_deg, buffer);
        internal::serialiseField<float>(ailerons_deg, buffer);
        internal::serialiseField<float>(elevator_deg, buffer);
        internal::serialiseField<float>(rudder_deg, buffer);

    }

    friend std::ostream& operator<<(std::ostream& os, const ActuatorCommands& msg)
    {

        os << "{ ";
        internal::printField<float>(os, "throttle_percent", msg.throttle_percent);
        internal::printField<float>(os, "flaps_deg", msg.flaps_deg);
        internal::printField<float>(os, "ailerons_deg", msg.ailerons_deg);
        internal::printField<float>(os, "elevator_deg", msg.elevator_deg);
        internal::printField<float>(os, "rudder_deg", msg.rudder_deg);

        os << " }";
        return os;
    }
};

}}

