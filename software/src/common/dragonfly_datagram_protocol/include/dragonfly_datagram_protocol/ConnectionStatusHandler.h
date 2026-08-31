#pragma once

#include "Serialisation.h"

#include <rpi_pico_utils/clock/ClockInterface.h>
#include <rpi_pico_utils/timer/Timer.h>

namespace dragonfly_datagram_protocol
{
    static constexpr uint32_t PING_PERIOD_MS = 100;
    static constexpr uint32_t CONNECTION_TIMEOUT_MS = 500;
    static constexpr uint16_t PING_HISTORY_SIZE = 30;

    class PingPongHistory
    {
    public:

        struct Entry
        {
            std::optional<uint64_t> ping_emission_stamp = std::nullopt;
            std::optional<uint64_t> pong_reception_stamp = std::nullopt;
        };

        void onPingSent(uint64_t time_us)
        {
            _history_index = (_history_index + 1) % PING_HISTORY_SIZE;
            _history[_history_index] = Entry
            {
                .ping_emission_stamp = time_us,
                .pong_reception_stamp = std::nullopt
            };
        }

        void onPong(const uint8_t& pong_sequence, uint8_t current_sequence, uint64_t time_us)
        {
            // If the ping is recent enough, update the history
            uint16_t sequence_delta = current_sequence - pong_sequence;
            if(sequence_delta * PING_PERIOD_MS <= CONNECTION_TIMEOUT_MS)
            { 
                uint32_t history_index = positiveModulus((int32_t)_history_index - (int32_t)sequence_delta, PING_HISTORY_SIZE);

                Entry& history_entry = _history[history_index];
                if(history_entry.ping_emission_stamp)
                {
                    history_entry.pong_reception_stamp = time_us;
                }
            }
        }

        dragonfly_msgs::msg::link_stats::ConnectionStatus getConnectionStatus(uint64_t time_us)
        {    
            uint32_t total_latency_ms = 0;
            uint32_t n_pongs_received = 0;
            uint32_t n_pings_lost = 0;
            uint32_t n_pings_sent = 0;
            std::optional<uint64_t> last_acknowledged_ping_stamp;

            for(uint16_t sequence_delta = 0; sequence_delta < PING_HISTORY_SIZE; sequence_delta++)
            {
                uint16_t history_index = positiveModulus((int32_t)_history_index - (int32_t)sequence_delta, PING_HISTORY_SIZE);
                Entry entry = _history[history_index];

                if(entry.ping_emission_stamp)
                {
                    uint64_t time_since_emission_us = time_us - entry.ping_emission_stamp.value();
                    if((time_since_emission_us / 1000) > CONNECTION_TIMEOUT_MS)
                    {
                        n_pings_sent++;
                        if(!entry.pong_reception_stamp)
                        {
                            n_pings_lost++;
                        }
                    }

                    if(entry.pong_reception_stamp)
                    {
                        n_pongs_received++;
                        total_latency_ms += (entry.pong_reception_stamp.value() - entry.ping_emission_stamp.value()) / 1000;
                        if(!last_acknowledged_ping_stamp)
                        {
                            last_acknowledged_ping_stamp = entry.ping_emission_stamp.value();
                        }
                    }
                }
            }

            if( n_pings_sent == 0 || 
                n_pongs_received == 0 || 
                !last_acknowledged_ping_stamp || 
                (time_us - last_acknowledged_ping_stamp.value()) / 1000 > CONNECTION_TIMEOUT_MS)
            {
                dragonfly_msgs::msg::link_stats::ConnectionStatus res;
                res.connected = false;
                res.packet_loss_percent = 100;
                res.ping_ms = 0;
                return res;
            }
            else
            {   
                dragonfly_msgs::msg::link_stats::ConnectionStatus res;
                res.connected = true;
                res.ping_ms = (n_pongs_received > 0) ? (total_latency_ms / n_pongs_received) : 0;
                res.packet_loss_percent = n_pings_sent > 0 ? (100.0f * n_pings_lost / n_pings_sent) : 100;
                return res;
            }
        }

    private:

        uint16_t positiveModulus(int32_t val, uint16_t mod)
        {
            return ((val % mod) + mod) % mod;
        }

        Entry _history[PING_HISTORY_SIZE];
        uint32_t _history_index = 0;
        
    };

    class ConnectionStatusHandler
    {
    public:

        ConnectionStatusHandler(ClockInterface* clock, PeerId peer_id) :
            _peer_id(peer_id), 
            _clock(clock),
            _send_ping_timer(clock, PING_PERIOD_MS) {}

        std::optional<PingPacket> poll()
        {
            if(_send_ping_timer.poll())
            {
                _sequence++;

                uint64_t time_us = _clock->getTime_us();
                _local_peer_via_local_link_history.onPingSent(time_us);
                _direct_remote_peer_via_local_link_history.onPingSent(time_us);
                _indirect_remote_peer_via_local_link_history.onPingSent(time_us);
                _direct_remote_peer_via_remote_link_history.onPingSent(time_us);
                _indirect_remote_peer_via_remote_link_history.onPingSent(time_us);

                return PingPacket 
                {
                    .src = _peer_id, 
                    .sequence = _sequence 
                };
            }

            return std::nullopt;
        }

        void onPong(uint8_t pong_sequence, PeerId src, LinkId link)
        {
            uint64_t time_us = _clock->getTime_us();
            PeerRelationship peer_relationship = getPeerRelationship(_peer_id, src);

            if(peer_relationship == PeerRelationship::LOCAL && link == LinkId::LOCAL)
            {
                _local_peer_via_local_link_history.onPong(pong_sequence, _sequence, time_us);
            }
            if(peer_relationship == PeerRelationship::DIRECT_REMOTE && link == LinkId::LOCAL)
            {
                _direct_remote_peer_via_local_link_history.onPong(pong_sequence, _sequence, time_us);
            }
            if(peer_relationship == PeerRelationship::INDIRECT_REMOTE && link == LinkId::LOCAL)
            {
                _indirect_remote_peer_via_remote_link_history.onPong(pong_sequence, _sequence, time_us);
            }
            if(peer_relationship == PeerRelationship::DIRECT_REMOTE && link == LinkId::REMOTE)
            {
                _direct_remote_peer_via_remote_link_history.onPong(pong_sequence, _sequence, time_us);
            }
            if(peer_relationship == PeerRelationship::INDIRECT_REMOTE && link == LinkId::REMOTE)
            {
                _indirect_remote_peer_via_remote_link_history.onPong(pong_sequence, _sequence, time_us);
            }
        }

        dragonfly_msgs::msg::link_stats::ConnectionsStatus getConnectionsStatus()
        {    
            uint64_t time_us = _clock->getTime_us();
            return dragonfly_msgs::msg::link_stats::ConnectionsStatus
            {
                .local_peer_via_local_link = _local_peer_via_local_link_history.getConnectionStatus(time_us),
                .direct_remote_peer_via_local_link = _direct_remote_peer_via_local_link_history.getConnectionStatus(time_us),
                .indirect_remote_peer_via_local_link = _indirect_remote_peer_via_local_link_history.getConnectionStatus(time_us),
                .direct_remote_peer_via_remote_link = _direct_remote_peer_via_remote_link_history.getConnectionStatus(time_us),
                .indirect_remote_peer_via_remote_link = _indirect_remote_peer_via_remote_link_history.getConnectionStatus(time_us),
            };
        }

    private:

        PeerId _peer_id;
        
        ClockInterface* _clock;
        Timer _send_ping_timer;

        uint8_t _sequence = 0;

        PingPongHistory _local_peer_via_local_link_history;
        PingPongHistory _direct_remote_peer_via_local_link_history;
        PingPongHistory _indirect_remote_peer_via_local_link_history;
        PingPongHistory _direct_remote_peer_via_remote_link_history;
        PingPongHistory _indirect_remote_peer_via_remote_link_history;
    };
}

