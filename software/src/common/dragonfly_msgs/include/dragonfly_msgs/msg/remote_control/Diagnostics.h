
#pragma once

#include <dragonfly_msgs/Utils.h>

#include <dragonfly_msgs/msg/link_stats/ConnectionsStatus.h>
#include <dragonfly_msgs/msg/remote_control/InputsDiagnostics.h>


namespace dragonfly_msgs { 
namespace msg { 
namespace remote_control { 


class Diagnostics
{
public:

    link_stats::ConnectionsStatus connections_status;
    remote_control::InputsDiagnostics inputs_diagnostics;


    static constexpr uint32_t _BodySize = link_stats::ConnectionsStatus::_BodySize + remote_control::InputsDiagnostics::_BodySize +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 6;

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
        internal::deserialiseField<link_stats::ConnectionsStatus>(connections_status, buffer);
        internal::deserialiseField<remote_control::InputsDiagnostics>(inputs_diagnostics, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<link_stats::ConnectionsStatus>(connections_status, buffer);
        internal::serialiseField<remote_control::InputsDiagnostics>(inputs_diagnostics, buffer);

    }

    void _print(uint32_t n_tabs = 0) const
    {
        char tabs[4 * 5 + 1] = {0};
        for(uint32_t i = 0; i < std::min(4 * n_tabs, (uint32_t)sizeof(tabs) - 1); i++)
        {
            tabs[i] = ' ';
        }

        std::cout << "{\n";
        internal::printField<link_stats::ConnectionsStatus>("connections_status", connections_status, n_tabs + 1);
        internal::printField<remote_control::InputsDiagnostics>("inputs_diagnostics", inputs_diagnostics, n_tabs + 1);

        std::cout << tabs << "}";
    }
};

}}}

