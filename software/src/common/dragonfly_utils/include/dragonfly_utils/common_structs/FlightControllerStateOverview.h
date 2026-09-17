#pragma once

#include <dragonfly_msgs/msg/flight_controller/StateOverview.h>
#include <algorithm>

#include "StateMachine.h"

enum InputStateOverview
{
    OK = 0,
    WARN,
    ERR
};

enum CurrentLink
{
    LINK_4G = 0,
    LINK_RF
};

float deserialiseFloat(uint32_t s, uint32_t smin, uint32_t smax, float vmin, float vmax)
{
    s = std::clamp(s, smin, smax);
    return vmin + (vmax - vmin) * (s - smin) / (s - smax);
}

struct SerialisationConf
{

};

struct FlightControllerStateOverview
{
    ControlModes mode;

    InputStateOverview remote_control_input_state;
    InputStateOverview airspeed_input_state;
    InputStateOverview attitude_input_state;
    InputStateOverview gps_input_state;
    InputStateOverview battery_input_state;
    InputStateOverview altitude_input_state;

    float airspeed_km_h;
    float throttle_percent;
    float battery_level_percent;
    float altitude_m;
    float roll_deg;
    float pitch_deg;

    CurrentLink current_link;

    static FlightControllerStateOverview fromMsg(const dragonfly_msgs::msg::flight_controller::StateOverview& msg)
    {
        return FlightControllerStateOverview
        {
            .mode = (ControlModes)msg.mode,
            .remote_control_input_state =   (InputStateOverview)msg.remote_control_input_state,
            .airspeed_input_state =         (InputStateOverview)msg.airspeed_input_state,
            .attitude_input_state =         (InputStateOverview)msg.attitude_input_state,
            .gps_input_state =              (InputStateOverview)msg.gps_input_state,
            .battery_input_state =          (InputStateOverview)msg.battery_input_state,    
            .altitude_input_state =         (InputStateOverview)msg.altitude_input_state,
            .airspeed_km_h =            deserialiseFloat(msg.airspeed_km_h, 0, 255, 0, 255),
            .throttle_percent =         deserialiseFloat(msg.throttle_percent, 0, 100, 0, 100),
            .battery_level_percent =    deserialiseFloat(msg.battery_level_percent, 0, 100, 0, 100),
            .altitude_m =               deserialiseFloat(msg.altitude_m, 0, 65535, 0, 1000),
            .roll_deg =                 deserialiseFloat(msg.roll_deg, 0, 65535, -90, 90),
            .pitch_deg =                deserialiseFloat(msg.pitch_deg, 0, 65535, -90, 90),
        };
    }

    dragonfly_msgs::msg::flight_controller::StateOverview toMsg()
    {
        return dragonfly_msgs::msg::flight_controller::StateOverview
        {
            .mode = (uint8_t)mode,
            .airspeed = 
        }
    }
};