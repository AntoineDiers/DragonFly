#pragma once

#include "ConnectionStatusHandler.h"
#include "TxBuffer.h"

#include <rpi_pico_utils/io/IoInterface.h>

namespace dragonfly_datagram_protocol
{
    class Multiplexer
    {
    public:

        Multiplexer(
            ClockInterface* clock,
            IoInterface* remote_io,
            IoInterface* local_io,
            PeerId peer_id):
            _peer_id(peer_id),
            _clock(clock),
            _remote_link 
            {
                remote_io,
                TxBuffer(clock)
            },
            _local_link
            {
                local_io,
                TxBuffer(clock)
            },
            _connections_status_handler(clock, peer_id) {}

        struct DatagramPair
        {
            std::optional<DatagramPacket> remote_link_datagram;
            std::optional<DatagramPacket> local_link_datagram;
        };

        DatagramPair poll()
        {
            std::optional<PingPacket> ping = _connections_status_handler.poll();
            if(ping)
            {
                _remote_link.tx_buffer.pushPing(ping.value());
                _local_link.tx_buffer.pushPing(ping.value());
            }

            return DatagramPair 
            {
                .remote_link_datagram = pollLink(LinkId::REMOTE),
                .local_link_datagram = pollLink(LinkId::LOCAL)
            };
        }

        template<typename T>
        void sendMessage(const T& msg, const PeerId& dst)
        {   
            DatagramPacket datagram;
            datagram.src = _peer_id;
            datagram.dst = dst;
            msg._serialise(datagram.data);

            _local_link.tx_buffer.pushDatagram(datagram);
            if(getPeerRelationship(_peer_id, dst) != PeerRelationship::LOCAL)
            {
                _remote_link.tx_buffer.pushDatagram(datagram);
            }
        };

        dragonfly_msgs::msg::link_stats::ConnectionsStatus getConnectionsStatus()
        {
            return _connections_status_handler.getConnectionsStatus();
        }

    private:

        static constexpr uint32_t RX_BUFFER_SIZE = 32;

        struct Link
        {
            IoInterface* io;
            TxBuffer tx_buffer;
            Buffer<RX_BUFFER_SIZE> rx_buffer = Buffer<RX_BUFFER_SIZE>();
            PacketData pending_tx_packet = PacketData();
            Deserialiser deserialiser = Deserialiser();
        };
        
        std::optional<DatagramPacket> pollLink(const LinkId& link_id)
        {
            Link& link = link_id == LinkId::REMOTE ? _remote_link : _local_link;
            Link& other_link = link_id == LinkId::REMOTE ? _local_link : _remote_link;

            // ------------------------------------------------
            // Transmit data from the tx buffer
            // ------------------------------------------------

            if(link.pending_tx_packet.size == 0)
            {
                link.pending_tx_packet = link.tx_buffer.getNextPacket().value_or(PacketData());
            }

            if(link.pending_tx_packet.size > 0)
            {   
                uint32_t n_bytes_sent;
                if(link.io->write(link.pending_tx_packet.data.data(), link.pending_tx_packet.size, n_bytes_sent) && n_bytes_sent > 0)
                {
                    uint32_t remaining_size = link.pending_tx_packet.size - n_bytes_sent;
                    std::memmove(link.pending_tx_packet.data.data(), link.pending_tx_packet.data.data() + n_bytes_sent, remaining_size);
                    link.pending_tx_packet.size = remaining_size;
                }
            }

            // ------------------------------------------------
            // Receive and process data
            // ------------------------------------------------

            if(link.rx_buffer.size == 0)
            {
                uint32_t n_bytes_receveived;
                if(link.io->read(link.rx_buffer.data.data(), RX_BUFFER_SIZE, n_bytes_receveived))
                {
                    link.rx_buffer.size = n_bytes_receveived;
                }
            }

            std::optional<Packet> packet;
            uint32_t n_bytes_processed = 0;
            while(n_bytes_processed < link.rx_buffer.size)
            {
                packet = link.deserialiser.feed(link.rx_buffer.data[n_bytes_processed]);
                n_bytes_processed++;
                if(packet) { break; }
            }

            uint32_t remaining_bytes = link.rx_buffer.size - n_bytes_processed;
            std::memmove(link.rx_buffer.data.data(), link.rx_buffer.data.data() + n_bytes_processed, remaining_bytes);
            link.rx_buffer.size = remaining_bytes;

            if(packet)
            {
                if(std::holds_alternative<PingPacket>(packet.value()))
                {
                    PingPacket ping = std::get<PingPacket>(packet.value());

                    // Answer
                    link.tx_buffer.pushPong(PongPacket 
                    { 
                        .src = _peer_id,
                        .dst = ping.src,
                        .sequence = ping.sequence 
                    });

                    // Forward if necessary
                    if(link_id == LinkId::REMOTE || (link_id == LinkId::LOCAL && getPeerRelationship(_peer_id, ping.src) == PeerRelationship::LOCAL))
                    {
                        other_link.tx_buffer.pushPing(ping);
                    }
                }
                if(std::holds_alternative<PongPacket>(packet.value()))
                {
                    PongPacket pong = std::get<PongPacket>(packet.value());
                    if(pong.dst == _peer_id)
                    {
                        _connections_status_handler.onPong(pong.sequence, pong.src, link_id);
                    }
                    else
                    {
                        other_link.tx_buffer.pushPong(pong);
                    }
                    
                }
                if(std::holds_alternative<DatagramPacket>(packet.value()))
                {
                    DatagramPacket datagram = std::get<DatagramPacket>(packet.value());

                    // Check if the packet must be forwarded
                    if(datagram.dst != _peer_id)
                    {
                        other_link.tx_buffer.pushDatagram(datagram);
                    }
                     
                    return datagram;
                }
            }
            
            return std::nullopt;
        }

        PeerId _peer_id;
        ClockInterface* _clock;
        Link _remote_link;
        Link _local_link;
        ConnectionStatusHandler _connections_status_handler;

        
    };
}