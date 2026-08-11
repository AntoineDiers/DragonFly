#pragma once

#include <rpi_pico_utils/clock/ClockInterface.h>

#include <dragonfly_msgs/msg/link_stats/Ping.h>
#include <dragonfly_msgs/msg/link_stats/Pong.h>
#include <dragonfly_msgs/msg/link_stats/LinkStats.h>

#include <config/common/comms/Config.h>

namespace dragonfly_datagram_protocol
{

    class LinkStatsHandler
    {
    public:

        dragonfly_msgs::msg::link_stats::Ping generatePing(ClockInterface* clock)
        {
            dragonfly_msgs::msg::link_stats::Ping res;
            res.stamp = clock->getTime_us();

            if(_n_sent_pings >= config::common::comms::PACKET_LOSS_WINDOW_SIZE)
            {
                std::memmove(_sent_pings, _sent_pings + sizeof(SentPing), 
                    (config::common::comms::PACKET_LOSS_WINDOW_SIZE - 1) * sizeof(SentPing));
                _n_sent_pings = config::common::comms::PACKET_LOSS_WINDOW_SIZE - 1;
            }

            _sent_pings[_n_sent_pings] = SentPing { res.stamp };

            _n_sent_pings++;

            return res;
        }

        dragonfly_msgs::msg::link_stats::Pong onPing(const dragonfly_msgs::msg::link_stats::Ping& ping)
        {
            dragonfly_msgs::msg::link_stats::Pong res;
            res.stamp = ping.stamp;
            return res;
        }

        void onPong(
            const dragonfly_msgs::msg::link_stats::Pong& pong,
            ClockInterface* clock)
        {
            for(uint32_t i = 0; i < _n_sent_pings; i++)
            {
                if(_sent_pings[i].stamp_us == pong.stamp)
                {
                    _sent_pings[i].pong_received = true;
                    _sent_pings[i].pong_stamp_us = clock->getTime_us();
                }
            }
        }

        dragonfly_msgs::msg::link_stats::LinkStats getLinkStats(ClockInterface* clock)
        {
            uint32_t total_latency_us = 0;
            uint32_t n_pings = 0;
            uint32_t n_pongs = 0;
            for(uint32_t i = 0; i < _n_sent_pings; i++)
            {
                SentPing sent_ping = _sent_pings[i];
                if((clock->getTime_us() - sent_ping.stamp_us) / 1000 < config::common::comms::PING_TTL_MS)
                {
                    break;
                }

                n_pings++;
                if(sent_ping.pong_received)
                {
                    n_pongs++;
                    total_latency_us += (sent_ping.pong_stamp_us - sent_ping.stamp_us);
                }
            }

            dragonfly_msgs::msg::link_stats::LinkStats res;
            if(n_pongs > 0)
            {
                res.ping_ms = (total_latency_us / 1000) / n_pongs;
                res.packet_loss_percent = (100 * (n_pings - n_pongs)) / n_pings;
            }
            else
            {
                res.ping_ms = 0;
                res.packet_loss_percent = 100;
            }
            
            return res;
        }

    private:

        struct SentPing
        {
            uint64_t stamp_us;
            bool pong_received = false;
            uint64_t pong_stamp_us = 0;
        };

        SentPing _sent_pings[config::common::comms::PACKET_LOSS_WINDOW_SIZE];
        uint32_t _n_sent_pings = 0;
    };
}

