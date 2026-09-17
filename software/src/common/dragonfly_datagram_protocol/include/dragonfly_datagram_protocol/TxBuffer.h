#pragma once

#include <rpi_pico_utils/clock/ClockInterface.h>
#include <rpi_pico_utils/timer/Timer.h>
#include <dragonfly_utils/Event.h>

#include "Serialisation.h"

namespace dragonfly_datagram_protocol
{
    static constexpr uint32_t BufferCapacityBytes = 1000;
    static constexpr uint32_t BufferCapacityPackets = 50;
    static constexpr uint32_t MaxPacketAgeMs = 300;

    class TxBuffer
    {
    public:

        struct Events
        {
            Event* unexpectedly_big_packet;
            Event* data_discarded;
        };

        TxBuffer(ClockInterface* clock, const Events& events) :
            _clock(clock),
            _events(events) {}

        static_assert(BufferCapacityBytes > MAX_PACKET_SIZE);
        static_assert(BufferCapacityPackets > 0);
    
        void pushDatagram(const DatagramPacket& datagram) { pushPacketData(serialiseDatagram(datagram)); }
        void pushPing(const PingPacket& ping) { pushPacketData(serialisePing(ping)); }
        void pushPong(const PongPacket& pong) { pushPacketData(serialisePong(pong)); }

        std::optional<PacketData> getNextPacket()
        {
            while(_n_packets > 0)
            {
                PacketInfo packet_info = _packets_info[0];
                uint64_t dt_ms = (_clock->getTime_us() - packet_info.stamp) / 1000;
                if(dt_ms > MaxPacketAgeMs)
                {
                    _events.data_discarded->trigger();
                    popFirstPacket();
                }

                PacketData res;
                res.size = packet_info.size;
                std::memcpy(res.data.data(), _buffer.data.data(), packet_info.size);
                popFirstPacket();

                return res;
            }

            return std::nullopt;
        }
        
    private:

        void pushPacketData(const PacketData& packet)
        {
            if(_n_packets == BufferCapacityPackets)
            {
                _events.data_discarded->trigger();
                popFirstPacket();
            }

            while(_buffer.size + packet.size > BufferCapacityBytes)
            {
                if(_n_packets == 0) 
                {
                    _events.unexpectedly_big_packet->trigger(); 
                    return; 
                }
                _events.data_discarded->trigger();
                popFirstPacket();
            }

            std::memcpy(_buffer.data.data(), packet.data.data(), packet.size);
            _buffer.size += packet.size;

            _packets_info[_n_packets] = PacketInfo
            {
                .size = packet.size,
                .stamp = _clock->getTime_us()
            };
            _n_packets++;
        }   

        void popFirstPacket()
        {
            if(_n_packets == 0) { return; }

            PacketInfo packet_info = _packets_info[0];

            std::memmove(_buffer.data.data(), _buffer.data.data() + packet_info.size, _buffer.size - packet_info.size);
            _buffer.size -= packet_info.size;

            std::memmove(_packets_info.data(), _packets_info.data() + 1, (_n_packets - 1) * sizeof(PacketInfo));
            _n_packets--;
        }

        struct PacketInfo
        {
            uint32_t size;
            uint64_t stamp;
        };

        ClockInterface* _clock;
        Events _events;

        Buffer<BufferCapacityBytes> _buffer;
        std::array<PacketInfo, BufferCapacityPackets> _packets_info;
        uint32_t _n_packets = 0;  
    };
}