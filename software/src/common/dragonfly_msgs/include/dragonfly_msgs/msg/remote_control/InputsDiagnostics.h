
#pragma once

#include <dragonfly_msgs/Utils.h>

#include <dragonfly_msgs/msg/PeriodicDataState.h>
#include <dragonfly_msgs/msg/PeriodicDataState.h>
#include <dragonfly_msgs/msg/PeriodicDataState.h>


namespace dragonfly_msgs { 
namespace msg { 
namespace remote_control { 


class InputsDiagnostics
{
public:

    PeriodicDataState flaps_knob_state;
    PeriodicDataState roll_stab_knob_state;
    PeriodicDataState pitch_stab_knob_state;


    static constexpr uint32_t _BodySize = PeriodicDataState::_BodySize + PeriodicDataState::_BodySize + PeriodicDataState::_BodySize +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 9;

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
        internal::deserialiseField<PeriodicDataState>(flaps_knob_state, buffer);
        internal::deserialiseField<PeriodicDataState>(roll_stab_knob_state, buffer);
        internal::deserialiseField<PeriodicDataState>(pitch_stab_knob_state, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<PeriodicDataState>(flaps_knob_state, buffer);
        internal::serialiseField<PeriodicDataState>(roll_stab_knob_state, buffer);
        internal::serialiseField<PeriodicDataState>(pitch_stab_knob_state, buffer);

    }

    void _print(uint32_t n_tabs = 0) const
    {
        char tabs[4 * 5 + 1] = {0};
        for(uint32_t i = 0; i < std::min(4 * n_tabs, (uint32_t)sizeof(tabs) - 1); i++)
        {
            tabs[i] = ' ';
        }

        std::cout << "{\n";
        internal::printField<PeriodicDataState>("flaps_knob_state", flaps_knob_state, n_tabs + 1);
        internal::printField<PeriodicDataState>("roll_stab_knob_state", roll_stab_knob_state, n_tabs + 1);
        internal::printField<PeriodicDataState>("pitch_stab_knob_state", pitch_stab_knob_state, n_tabs + 1);

        std::cout << tabs << "}";
    }
};

}}}

