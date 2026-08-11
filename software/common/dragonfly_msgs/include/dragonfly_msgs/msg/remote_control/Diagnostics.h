
#pragma once

#include <dragonfly_msgs/Utils.h>

#include <dragonfly_msgs/msg/remote_control/LinksDiagnostics.h>
#include <dragonfly_msgs/msg/remote_control/InputsDiagnostics.h>


namespace dragonfly_msgs { 
namespace msg { 
namespace remote_control { 


class Diagnostics
{
public:

    uint32_t n_restarts;
    remote_control::LinksDiagnostics links_diagnostics;
    remote_control::InputsDiagnostics inputs_diagnostics;


    static constexpr uint32_t _BodySize = sizeof(uint32_t) + remote_control::LinksDiagnostics::_BodySize + remote_control::InputsDiagnostics::_BodySize +  0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = 5;

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
        internal::deserialiseField<uint32_t>(n_restarts, buffer);
        internal::deserialiseField<remote_control::LinksDiagnostics>(links_diagnostics, buffer);
        internal::deserialiseField<remote_control::InputsDiagnostics>(inputs_diagnostics, buffer);

    }

    void _serialiseFields(uint8_t* &buffer) const
    {
        internal::serialiseField<uint32_t>(n_restarts, buffer);
        internal::serialiseField<remote_control::LinksDiagnostics>(links_diagnostics, buffer);
        internal::serialiseField<remote_control::InputsDiagnostics>(inputs_diagnostics, buffer);

    }

    friend std::ostream& operator<<(std::ostream& os, const Diagnostics& msg)
    {

        os << "{ ";
        internal::printField<uint32_t>(os, "n_restarts", msg.n_restarts);
        internal::printField<remote_control::LinksDiagnostics>(os, "links_diagnostics", msg.links_diagnostics);
        internal::printField<remote_control::InputsDiagnostics>(os, "inputs_diagnostics", msg.inputs_diagnostics);

        os << " }";
        return os;
    }
};

}}}

