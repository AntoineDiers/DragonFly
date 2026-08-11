
#pragma once

#include <dragonfly_msgs/Utils.h>

#include <dragonfly_msgs/msg/PeriodicDataState.h>
#include <dragonfly_msgs/msg/PeriodicDataState.h>
#include <dragonfly_msgs/msg/PeriodicDataState.h>
#include <dragonfly_msgs/msg/PeriodicDataState.h>
#include <dragonfly_msgs/msg/PeriodicDataState.h>


namespace dragonfly_msgs { 
namespace msg { 


class FlightControllerInputStates
{
public:

    PeriodicDataState remote_control;
    PeriodicDataState primary_attitude;
    PeriodicDataState backup_attitude;
    PeriodicDataState altitude;
    PeriodicDataState battery_level;


    static constexpr uint32_t _BodySize = PeriodicDataState::_BodySize + PeriodicDataState::_BodySize + PeriodicDataState::_BodySize + PeriodicDataState::_BodySize + PeriodicDataState::_BodySize +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 0;

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
        internal::deserialiseField<PeriodicDataState>(remote_control, buffer);
        internal::deserialiseField<PeriodicDataState>(primary_attitude, buffer);
        internal::deserialiseField<PeriodicDataState>(backup_attitude, buffer);
        internal::deserialiseField<PeriodicDataState>(altitude, buffer);
        internal::deserialiseField<PeriodicDataState>(battery_level, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<PeriodicDataState>(remote_control, buffer);
        internal::serialiseField<PeriodicDataState>(primary_attitude, buffer);
        internal::serialiseField<PeriodicDataState>(backup_attitude, buffer);
        internal::serialiseField<PeriodicDataState>(altitude, buffer);
        internal::serialiseField<PeriodicDataState>(battery_level, buffer);

    }

    friend std::ostream& operator<<(std::ostream& os, const FlightControllerInputStates& msg)
    {

        os << "{ ";
        internal::printField<PeriodicDataState>(os, "remote_control", msg.remote_control);
        internal::printField<PeriodicDataState>(os, "primary_attitude", msg.primary_attitude);
        internal::printField<PeriodicDataState>(os, "backup_attitude", msg.backup_attitude);
        internal::printField<PeriodicDataState>(os, "altitude", msg.altitude);
        internal::printField<PeriodicDataState>(os, "battery_level", msg.battery_level);

        os << " }";
        return os;
    }
};

}}

