
#pragma once

#include <dragonfly_msgs/msgs/flight_controller/Actuators.h>
#include <dragonfly_msgs/msgs/flight_controller/Errors.h>
#include <dragonfly_msgs/msgs/flight_controller/InputDiagsOverview.h>
#include <dragonfly_msgs/msgs/flight_controller/Navigation.h>
#include <dragonfly_msgs/msgs/flight_controller/StateOverview.h>
#include <dragonfly_msgs/msgs/comms/DataRates.h>
#include <dragonfly_msgs/msgs/comms/ConnectionsStatus.h>
#include <dragonfly_msgs/msgs/comms/ConnectionStatus.h>
#include <dragonfly_msgs/msgs/remote_control/Inputs.h>
#include <dragonfly_msgs/enums/ErrorState.h>
#include <dragonfly_msgs/enums/ControlModes.h>
#include <dragonfly_msgs/enums/DiagLevel.h>


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
    
    static constexpr uint32_t MAX_MSG_SIZE = internal::max(
        dragonfly_msgs::msgs::flight_controller::Actuators::_SizeBytes,
        dragonfly_msgs::msgs::flight_controller::Errors::_SizeBytes,
        dragonfly_msgs::msgs::flight_controller::InputDiagsOverview::_SizeBytes,
        dragonfly_msgs::msgs::flight_controller::Navigation::_SizeBytes,
        dragonfly_msgs::msgs::flight_controller::StateOverview::_SizeBytes,
        dragonfly_msgs::msgs::comms::DataRates::_SizeBytes,
        dragonfly_msgs::msgs::comms::ConnectionsStatus::_SizeBytes,
        dragonfly_msgs::msgs::comms::ConnectionStatus::_SizeBytes,
        dragonfly_msgs::msgs::remote_control::Inputs::_SizeBytes,
        0);
}
