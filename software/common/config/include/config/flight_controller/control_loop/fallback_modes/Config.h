#pragma once

namespace config::flight_controller::control_loop::fallback_modes
{
    static constexpr float SAFE_ALTITDE_LIMIT_M = 20;
    static constexpr float SAFE_BATTERY_LEVEL_PERCENT = 20;

    namespace closed_loop_descent
    {
        static constexpr float THROTTLE_PERCENT = 30;
        static constexpr float FLAPS_DEG = 0;
        static constexpr float PITCH_SETPOINT_DEG = 5;
    } 
    namespace closed_loop_landing
    {
        static constexpr float MAX_THROTTLE_PERCENT = 30;
        static constexpr float MIN_THROTTLE_ALTITUDE_M = 3;
        static constexpr float FLAPS_DEG = 0;
        static constexpr float PITCH_SETPOINT_DEG = 5;
    }
    namespace closed_loop_orbit
    {
        static constexpr float THROTTLE_PERCENT = 30;
        static constexpr float FLAPS_DEG = 0;
        static constexpr float ROLL_SETPOINT_DEG = 10;
        static constexpr float ALTITUDE_SETPOINT_M = 50;
        static constexpr float MAX_PITCH_SETPOINT_DEG = 5;
        static constexpr float RUDDER_DEG = 10;
    }
    namespace open_loop_descent
    {
        static constexpr float THROTTLE_PERCENT = 30;
        static constexpr float FLAPS_DEG = 0;
        static constexpr float ELEVATOR_DEG = 5;
    }
    namespace open_loop_glide
    {
        static constexpr float THROTTLE_PERCENT = 30;
        static constexpr float FLAPS_DEG = 0;
        static constexpr float ELEVATOR_DEG = 5;
    }
    namespace open_loop_landing
    {
        static constexpr float MAX_THROTTLE_PERCENT = 30;
        static constexpr float MIN_THROTTLE_ALTITUDE_M = 3;
        static constexpr float FLAPS_DEG = 20;
        static constexpr float ELEVATOR_DEG = 5;
    }
};