#pragma once

#include <rpi_pico_utils/utils/Periodic.h>
#include <dragonfly_msgs/msgs.h>

#include <config/flight_controller/control_loop/inputs/Config.h>

namespace control_loop
{
    class Inputs
    {
    public:

        Inputs(ClockInterface* clock) : 
            remote_control  (config::flight_controller::control_loop::inputs::REMOTE_CONTROL,      clock),
            primary_attitude(config::flight_controller::control_loop::inputs::PRIMARY_ATTITUDE,    clock),
            backup_attitude (config::flight_controller::control_loop::inputs::BACKUP_ATTITUDE,     clock),
            altitude        (config::flight_controller::control_loop::inputs::ALTITUDE,            clock),
            battery_level   (config::flight_controller::control_loop::inputs::BATTERY_LEVEL,       clock) {}

        dragonfly_msgs::msg::FlightControllerInputStates getState()
        {
            dragonfly_msgs::msg::FlightControllerInputStates res;
            res.remote_control =    remote_control.getStateMsg();
            res.primary_attitude =  primary_attitude.getStateMsg();
            res.backup_attitude =   backup_attitude.getStateMsg();
            res.altitude =          altitude.getStateMsg();
            res.battery_level =     battery_level.getStateMsg();
            return res;
        }

        Periodic<dragonfly_msgs::msg::RemoteControlInputs> remote_control;
        Periodic<dragonfly_msgs::msg::Attitude> primary_attitude;
        Periodic<dragonfly_msgs::msg::Attitude> backup_attitude;
        Periodic<dragonfly_msgs::msg::Altitude> altitude;
        Periodic<dragonfly_msgs::msg::BatteryLevel> battery_level; 
    };
}
