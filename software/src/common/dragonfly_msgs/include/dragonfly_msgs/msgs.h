
#pragma once

#include <dragonfly_msgs/Utils.h>
#include <dragonfly_msgs/msg/FlightControllerInputStates.h>
#include <dragonfly_msgs/msg/BatteryLevel.h>
#include <dragonfly_msgs/msg/PeriodicDataState.h>
#include <dragonfly_msgs/msg/ActuatorCommands.h>
#include <dragonfly_msgs/msg/BatteryStats.h>
#include <dragonfly_msgs/msg/Altitude.h>
#include <dragonfly_msgs/msg/remote_control/Diagnostics.h>
#include <dragonfly_msgs/msg/remote_control/LinksDiagnostics.h>
#include <dragonfly_msgs/msg/remote_control/Inputs.h>
#include <dragonfly_msgs/msg/remote_control/InputsDiagnostics.h>
#include <dragonfly_msgs/msg/link_stats/Pong.h>
#include <dragonfly_msgs/msg/link_stats/ConnectionsStatus.h>
#include <dragonfly_msgs/msg/link_stats/FlightControllerLinkStats.h>
#include <dragonfly_msgs/msg/link_stats/ConnectionStatus.h>
#include <dragonfly_msgs/msg/link_stats/LinkStats.h>
#include <dragonfly_msgs/msg/link_stats/Ping.h>
#include <dragonfly_msgs/msg/Attitude.h>


namespace dragonfly_msgs
{
    namespace internal
    {
        constexpr uint32_t max(uint32_t a, uint32_t b)
        {
            return a > b ? a : b;
        }

        template<typename... Ts>
        constexpr uint32_t max(uint32_t a, uint32_t b, Ts... rest)
        {
            return max(max(a, b), rest...);
        }
    }
    
    static constexpr uint32_t MAX_MSG_SIZE = sizeof(MsgId) + internal::max(
        dragonfly_msgs::msg::FlightControllerInputStates::_BodySize,
        dragonfly_msgs::msg::BatteryLevel::_BodySize,
        dragonfly_msgs::msg::PeriodicDataState::_BodySize,
        dragonfly_msgs::msg::ActuatorCommands::_BodySize,
        dragonfly_msgs::msg::BatteryStats::_BodySize,
        dragonfly_msgs::msg::Altitude::_BodySize,
        dragonfly_msgs::msg::remote_control::Diagnostics::_BodySize,
        dragonfly_msgs::msg::remote_control::LinksDiagnostics::_BodySize,
        dragonfly_msgs::msg::remote_control::Inputs::_BodySize,
        dragonfly_msgs::msg::remote_control::InputsDiagnostics::_BodySize,
        dragonfly_msgs::msg::link_stats::Pong::_BodySize,
        dragonfly_msgs::msg::link_stats::ConnectionsStatus::_BodySize,
        dragonfly_msgs::msg::link_stats::FlightControllerLinkStats::_BodySize,
        dragonfly_msgs::msg::link_stats::ConnectionStatus::_BodySize,
        dragonfly_msgs::msg::link_stats::LinkStats::_BodySize,
        dragonfly_msgs::msg::link_stats::Ping::_BodySize,
        dragonfly_msgs::msg::Attitude::_BodySize,
        0);
}
