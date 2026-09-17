#pragma once

#include <nlohmann/json.hpp>

#include <dragonfly_msgs/msgs/flight_controller/StateOverview.h>

class Backend
{
public:

    Backend(){}

    void tick(){}

    std::optional<nlohmann::json> getFlightControllerStateOverview()
    {
        if(!_state_overview) { return std::nullopt; }

        return _state_overview->_serialiseJson();
    }

private:

    std::optional<dragonfly_msgs::msgs::flight_controller::StateOverview> _state_overview = 
    dragonfly_msgs::msgs::flight_controller::StateOverview
    {
        .control_mode = dragonfly_msgs::enums::ControlModes::MANUAL,
        .actuators = 
        {
            .throttle_percent = 35,
            .left_flaperon_deg = 10,
            .right_flaperon_deg = 0,
            .left_ruddervator_deg = -5,
            .right_ruddervator_deg = 35
        },
        .navigation = 
        {
            .roll_deg = 14,
            .pitch_deg = -15,
            .airspeed_km_h = 50,
            .altitude_m = 250
        },
        .input_diags = 
        {
            .remote_control = dragonfly_msgs::enums::DiagLevel::OK,
            .airspeed = dragonfly_msgs::enums::DiagLevel::ERR,
            .attitude = dragonfly_msgs::enums::DiagLevel::WARN,
            .altitude = dragonfly_msgs::enums::DiagLevel::OK,
            .gps = dragonfly_msgs::enums::DiagLevel::OK,
            .battery_level = dragonfly_msgs::enums::DiagLevel::OK,
        },
        .battery_level_percent = 85
    };
};