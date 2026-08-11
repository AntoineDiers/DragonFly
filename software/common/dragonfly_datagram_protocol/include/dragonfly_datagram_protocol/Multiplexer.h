#pragma once

#include "LinkStatsHandler.h"
#include "DragonflyDatagramProtocol.h"

#include <dragonfly_msgs/msg/link_stats/LinkStats.h>
#include <dragonfly_msgs/msg/link_stats/Ping.h>
#include <dragonfly_msgs/msg/link_stats/Pong.h>
#include <rpi_pico_utils/io/IoInterface.h>
#include <rpi_pico_utils/timer/Timer.h>

#include <optional>

namespace dragonfly_datagram_protocol
{
    template<Node OwnPeerId, Node LocalPeerId, Node DirectRemotePeerId, Node IndirectRemotePeerId>
    class Multiplexer
    {
    public:

        static_assert(OwnPeerId != LocalPeerId);
        static_assert(OwnPeerId != DirectRemotePeerId);
        static_assert(OwnPeerId != IndirectRemotePeerId);
        static_assert(LocalPeerId != DirectRemotePeerId);
        static_assert(LocalPeerId != IndirectRemotePeerId);
        static_assert(DirectRemotePeerId != IndirectRemotePeerId);

        typedef Deserialiser::Datagram Datagram;

        struct LinkStats
        {
            dragonfly_msgs::msg::link_stats::LinkStats local_peer;
            dragonfly_msgs::msg::link_stats::LinkStats direct_remote_peer_via_local_link;
            dragonfly_msgs::msg::link_stats::LinkStats direct_remote_peer_via_remote_link;
            dragonfly_msgs::msg::link_stats::LinkStats indirect_remote_peer_via_local_link;
            dragonfly_msgs::msg::link_stats::LinkStats indirect_remote_peer_via_remote_link;
        };

        Multiplexer(ClockInterface* clock, IoInterface* local_io, IoInterface* remote_io) : 
            _send_pings_timer(clock, config::common::comms::PING_PERIOD_MS),
            _clock(clock)
        {
            _remote_link.link.io = remote_io;
            _local_link.link.io = local_io;

            _current_link_to_direct_remote_peer = &_remote_link.link;
            _current_link_to_indirect_remote_peer = &_remote_link.link;
        }

        std::optional<Datagram> poll()
        {
            std::optional<Datagram> res;

            // Select the best available links to the remote peers
            selectBestLinksToRemotePeers();

            // Generate ping messages when necessary
            if(_send_pings_timer.poll())
            {
                sendMessage(_local_link.local_peer_stats_handler.generatePing(_clock),              _local_link.link.serialiser,     LocalPeerId);
                sendMessage(_local_link.direct_remote_peer_stats_handler.generatePing(_clock),      _local_link.link.serialiser,     DirectRemotePeerId);
                sendMessage(_local_link.indirect_remote_peer_stats_handler.generatePing(_clock),    _local_link.link.serialiser,     IndirectRemotePeerId);
                sendMessage(_remote_link.direct_remote_peer_stats_handler.generatePing(_clock),     _remote_link.link.serialiser,    DirectRemotePeerId);
                sendMessage(_remote_link.indirect_remote_peer_stats_handler.generatePing(_clock),   _remote_link.link.serialiser,    IndirectRemotePeerId);
            }

            // Empty Links Tx buffers when possible
            sendLinkTx(_local_link.link);
            sendLinkTx(_remote_link.link);

            // Poll Remote Link
            uint32_t n_bytes_received;
            Buffer<dragonfly_datagram_protocol::MAX_PACKET_SIZE> packet;
            if(_remote_link.link.io->read(packet.data.data(), dragonfly_datagram_protocol::MAX_PACKET_SIZE, n_bytes_received)) 
            {
                std::optional<Datagram> datagram = _remote_link.link.deserialiser.feed(packet.data.data(), n_bytes_received);
                if(datagram)
                {
                    // Handle messages addressed to us
                    if(datagram->dst == OwnPeerId)
                    {
                        // Handle ping messages
                        dragonfly_msgs::msg::link_stats::Ping ping;
                        dragonfly_msgs::msg::link_stats::Pong pong;
                        if(ping._deserialise(datagram->payload))
                        {
                            if(datagram->src == DirectRemotePeerId)
                            {
                                pong = _remote_link.direct_remote_peer_stats_handler.onPing(ping);
                                sendMessage(pong, _remote_link.link.serialiser, datagram->src);
                            }
                            if(datagram->src == IndirectRemotePeerId)
                            {
                                pong = _remote_link.indirect_remote_peer_stats_handler.onPing(ping);
                                sendMessage(pong, _remote_link.link.serialiser, datagram->src);
                            }
                        }
                        // Handle Pong Messages
                        else if(pong._deserialise(datagram->payload))
                        {
                            if(datagram->src == DirectRemotePeerId)
                            {
                                _remote_link.direct_remote_peer_stats_handler.onPong(pong, _clock);
                            }
                            if(datagram->src == IndirectRemotePeerId)
                            {
                                _remote_link.indirect_remote_peer_stats_handler.onPong(pong, _clock);
                            }
                        }
                        // Handle other messages
                        else
                        {
                            res = datagram;
                        }
                    }
                    // Forward messages that are not only addressed to us on the local link
                    else
                    {
                        _local_link.link.serialiser.feedDatagram(res->payload, res->src, res->dst);
                    }
                }
            }

            // Poll local link if nothing was received on remote link
            if(!res && _local_link.link.io->read(packet.data.data(), dragonfly_datagram_protocol::MAX_PACKET_SIZE, n_bytes_received)) 
            {
                std::optional<Datagram> datagram = _local_link.link.deserialiser.feed(packet.data.data(), n_bytes_received);
                if(datagram)
                {
                    // Handle messages addressed to us
                    if(datagram->dst == OwnPeerId)
                    {
                        // Handle ping messages
                        dragonfly_msgs::msg::link_stats::Ping ping;
                        dragonfly_msgs::msg::link_stats::Pong pong;
                        if(ping._deserialise(datagram->payload))
                        {
                            if(datagram->src == LocalPeerId)
                            {
                                pong = _local_link.local_peer_stats_handler.onPing(ping);
                                sendMessage(pong, _local_link.link.serialiser, datagram->src);
                            }
                            if(datagram->src == DirectRemotePeerId)
                            {
                                pong = _local_link.direct_remote_peer_stats_handler.onPing(ping);
                                sendMessage(pong, _local_link.link.serialiser, datagram->src);
                            }
                            if(datagram->src == IndirectRemotePeerId)
                            {
                                pong = _local_link.indirect_remote_peer_stats_handler.onPing(ping);
                                sendMessage(pong, _local_link.link.serialiser, datagram->src);
                            }
                        }
                        // Handle Pong Messages
                        else if(pong._deserialise(datagram->payload))
                        {
                            if(datagram->src == LocalPeerId)
                            {
                                _local_link.local_peer_stats_handler.onPong(pong, _clock);
                            }
                            if(datagram->src == DirectRemotePeerId)
                            {
                                _local_link.direct_remote_peer_stats_handler.onPong(pong, _clock);
                            }
                            if(datagram->src == IndirectRemotePeerId)
                            {
                                _local_link.indirect_remote_peer_stats_handler.onPong(pong, _clock);
                            }
                        }
                        // Handle other messages
                        else
                        {
                            res = datagram;
                        }
                    }
                    // Forward messages that are not only addressed to us on the local link
                    else
                    {
                        _remote_link.link.serialiser.feedDatagram(res->payload, res->src, res->dst);
                    }
                }
            }

            return res;
        }

        template<Node PeerId, typename T>
        void send(const T& msg)
        {
            if constexpr (PeerId == LocalPeerId)
            {
                sendMessage(msg, _local_link.link.serialiser, PeerId);
            }
            else if constexpr (PeerId == DirectRemotePeerId)
            {
                sendMessage(msg, _current_link_to_direct_remote_peer->serialiser, PeerId);
            }
            else if constexpr (PeerId == DirectRemotePeerId)
            {
                sendMessage(msg, _current_link_to_indirect_remote_peer->serialiser, PeerId);
            }
            else
            {
                static_assert(false);
            }
        }

        template<Node PeerId> 
        dragonfly_msgs::msg::link_stats::LinkStats getLinkStats()
        {
            if constexpr (PeerId == LocalPeerId)
            {
                return _local_link.local_peer_stats_handler.getLinkStats(_clock);
            }
            else if constexpr (PeerId == DirectRemotePeerId)
            {
                if(_current_link_to_direct_remote_peer == &_local_link.link)
                {
                    return _local_link.direct_remote_peer_stats_handler.getLinkStats(_clock);
                }
                else
                {
                    return _remote_link.direct_remote_peer_stats_handler.getLinkStats(_clock);
                }
            }
            else if constexpr (PeerId == IndirectRemotePeerId)
            {
                if(_current_link_to_indirect_remote_peer == &_local_link.link)
                {
                    return _local_link.indirect_remote_peer_stats_handler.getLinkStats(_clock);
                }
                else
                {
                    return _remote_link.indirect_remote_peer_stats_handler.getLinkStats(_clock);
                }
            }
            else
            {
                static_assert(false);
            }
        }

        LinkStats getFullLinkStats()
        {
            LinkStats res;
            res.local_peer = _local_link.local_peer_stats_handler.getLinkStats(_clock);
            res.direct_remote_peer_via_local_link = _local_link.direct_remote_peer_stats_handler.getLinkStats(_clock);
            res.direct_remote_peer_via_remote_link = _remote_link.direct_remote_peer_stats_handler.getLinkStats(_clock);
            res.indirect_remote_peer_via_local_link = _local_link.indirect_remote_peer_stats_handler.getLinkStats(_clock);
            res.indirect_remote_peer_via_remote_link = _remote_link.indirect_remote_peer_stats_handler.getLinkStats(_clock);
            return res;
        }

    private:

        struct Link
        {
            IoInterface* io;
            dragonfly_datagram_protocol::Deserialiser deserialiser;
            dragonfly_datagram_protocol::Serialiser serialiser;
            Buffer<dragonfly_datagram_protocol::MAX_PACKET_SIZE> pending_packet;
        };

        struct LocalLink
        {
            Link link;

            LinkStatsHandler local_peer_stats_handler;
            LinkStatsHandler direct_remote_peer_stats_handler;
            LinkStatsHandler indirect_remote_peer_stats_handler;
        };

        struct RemoteLink
        {
            Link link;

            LinkStatsHandler direct_remote_peer_stats_handler;
            LinkStatsHandler indirect_remote_peer_stats_handler;
        };

        template<typename T>
        void sendMessage(
            const T& msg, 
            dragonfly_datagram_protocol::Serialiser& serialiser,
            uint8_t dst)
        {
            Buffer<T::_SerialisationSize> payload;
            msg._serialise(payload);
            serialiser.feedDatagram(payload, OwnPeerId, dst);
        }

        void sendLinkTx(Link& link)
        {
            // Try sending data from the Tx buffer on this link
            while(link.io->canWrite())
            {
                if(link.pending_packet.size == 0)
                {
                    std::optional<Buffer<dragonfly_datagram_protocol::MAX_PACKET_SIZE>> next_packet = link.serialiser.getNextPacket();
                    if(!next_packet) { break; }
                    link.pending_packet = next_packet.value(); 
                }

                uint32_t n_bytes_written;
                if(!link.io->write(link.pending_packet.data.data(), link.pending_packet.size, n_bytes_written))
                {
                    break;
                }

                std::memmove(link.pending_packet.data.data(), link.pending_packet.data.data() + n_bytes_written, link.pending_packet.size - n_bytes_written);
                link.pending_packet.size -= n_bytes_written;
            }
        }

        void selectBestLinksToRemotePeers()
        {
            // TODO
        }

        LocalLink _local_link;
        RemoteLink _remote_link;

        Link* _current_link_to_direct_remote_peer;
        Link* _current_link_to_indirect_remote_peer;

        Timer _send_pings_timer;
        ClockInterface* _clock;        
    };
}

