#pragma once

#include <cstdint>

namespace config::flight_controller::actuators::limits
{
    static constexpr float MAX_THROTTLE_PERCENT = 100;
    static constexpr float MAX_FLAP_ANGLE_DEG = 45;
    static constexpr float MAX_AILERON_ANGLE_DEG = 30;
    static constexpr float MAX_FLAPERON_ANGLE_DEG = 45;
    static constexpr float MAX_RUDDERVATOR_ANGLE_DEG = 30;
    static constexpr float MAX_ELEVATOR_ANGLE_DEG = 30;
    static constexpr float MAX_RUDDER_ANGLE_DEG = 30;
}