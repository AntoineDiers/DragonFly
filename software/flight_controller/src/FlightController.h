#pragma once

#include <algorithm>
#include <iostream>
#include <math.h>

#include <config/flight_controller/actuators/Config.h>
#include <config/flight_controller/actuators/limits/Config.h>
#include <config/flight_controller/comms/Config.h>
#include <config/flight_controller/control_loop/Config.h>
#include <config/flight_controller/imu/Config.h>

#include "imu_driver/ImuDriver.h"
#include "control_loop/ControlLoop.h"

#include <rpi_pico_utils/timer/Timer.h>
#include <dragonfly_datagram_protocol/Multiplexer.h>

class FlightController
{
public:

    FlightController(HardwareInterface hw_interface) : 
        _hw_interface(hw_interface),
        _comms_multiplexer(hw_interface.clock, hw_interface.pc_uart, hw_interface.rf_uart),
        _control_loop(hw_interface.clock),
        _imu_driver(hw_interface),
        _control_loop_timer(hw_interface.clock, config::flight_controller::control_loop::PERIOD_MS) {}

    void tick()
    {
        // Poll and handle incoming messages
        std::optional<Datagram> incoming_datagram = _comms_multiplexer.poll();
        if(incoming_datagram) { onMessage(incoming_datagram.value()); }

        // Poll IMU data
        std::optional<dragonfly_msgs::msg::Attitude> attitude = _imu_driver.poll();
        if(attitude) { onPrimaryImuAttitude(attitude.value()); } 

        // Run state machine every config::flight_controller::CONTROL_PERIOD_US
        if(_control_loop_timer.poll())
        {
            sendActuatorCommands(_control_loop.tick());

            dragonfly_msgs::msg::FlightControllerInputStates input_states = _control_loop.inputs().getState();
            
            std::cout << "Input States : " << "\n";
            std::cout << " - remote_control :   " << (int)input_states.remote_control.state << " " << input_states.remote_control.rate_hz << " Hz " << input_states.remote_control.time_since_last_ms << " ms\n";
            std::cout << " - primary_attitude : " << (int)input_states.primary_attitude.state << " " << input_states.primary_attitude.rate_hz << " Hz " << input_states.primary_attitude.time_since_last_ms << " ms\n";
            std::cout << " - backup_attitude :  " << (int)input_states.backup_attitude.state << " " << input_states.backup_attitude.rate_hz << " Hz " << input_states.backup_attitude.time_since_last_ms << " ms\n";
            std::cout << " - altitude :         " << (int)input_states.altitude.state << " " << input_states.altitude.rate_hz << " Hz " << input_states.altitude.time_since_last_ms << " ms\n";
            std::cout << " - battery_level :    " << (int)input_states.battery_level.state << " " << input_states.battery_level.rate_hz << " Hz " << input_states.battery_level.time_since_last_ms << " ms\n";
        }
    }

private:

    typedef dragonfly_datagram_protocol::Deserialiser::Datagram Datagram;

    void sendActuatorCommands(dragonfly_msgs::msg::ActuatorCommands commands)
    {
        // Replace the ESC dead zone with our own
        float throttle_percent = commands.throttle_percent < config::flight_controller::actuators::esc::DEAD_ZONE_PERCENT ? 0 : 
            config::flight_controller::actuators::esc::MIN_THROTTLE_PERCENT + (throttle_percent - config::flight_controller::actuators::esc::DEAD_ZONE_PERCENT) * 
            (100.0f - config::flight_controller::actuators::esc::MIN_THROTTLE_PERCENT) / (100.0f - config::flight_controller::actuators::esc::DEAD_ZONE_PERCENT);

        // Compute flaperons angles from flaps and ailerons commands
        float left_flaperon_cmd = commands.flaps_deg + commands.ailerons_deg;
        float right_flaperon_cmd = commands.flaps_deg - commands.ailerons_deg;

        throttle_percent =          std::clamp(commands.throttle_percent,   0.0f,                                               config::flight_controller::actuators::limits::MAX_THROTTLE_PERCENT);
        left_flaperon_cmd =         std::clamp(left_flaperon_cmd,           -config::flight_controller::actuators::limits::MAX_FLAPERON_ANGLE_DEG, config::flight_controller::actuators::limits::MAX_FLAPERON_ANGLE_DEG);
        right_flaperon_cmd =        std::clamp(right_flaperon_cmd,          -config::flight_controller::actuators::limits::MAX_FLAPERON_ANGLE_DEG, config::flight_controller::actuators::limits::MAX_FLAPERON_ANGLE_DEG);
        commands.elevator_deg =     std::clamp(commands.elevator_deg,       -config::flight_controller::actuators::limits::MAX_ELEVATOR_ANGLE_DEG, config::flight_controller::actuators::limits::MAX_ELEVATOR_ANGLE_DEG);
        commands.rudder_deg =       std::clamp(commands.rudder_deg,         -config::flight_controller::actuators::limits::MAX_RUDDER_ANGLE_DEG,   config::flight_controller::actuators::limits::MAX_RUDDER_ANGLE_DEG);
    
        _hw_interface.esc_pwm->setCommandPercent(throttle_percent);
        sendServoCommand(_hw_interface.left_flaperon_pwm, left_flaperon_cmd);
        sendServoCommand(_hw_interface.right_flaperon_pwm, right_flaperon_cmd);
        sendServoCommand(_hw_interface.elevator_pwm, commands.elevator_deg);
        sendServoCommand(_hw_interface.rudder_pwm, commands.rudder_deg);
    }

    void sendServoCommand(PwmInterface* pwm, float angle_deg)
    {
        (void) pwm;
        (void) angle_deg;
        // TODO
    }

    void onMessage(const Datagram& incoming_datagram)
    {
        if(incoming_datagram.src == dragonfly_datagram_protocol::Node::PAYLOAD_COMPUTER)
        {
            {
                dragonfly_msgs::msg::Attitude msg;
                if(msg._deserialise(incoming_datagram.payload)) { onBackupImuAttitude(msg); return; }
            }
        }
        if(incoming_datagram.src == dragonfly_datagram_protocol::Node::GROUND_STATION)
        {
            {
                dragonfly_msgs::msg::RemoteControlInputs msg;
                if(msg._deserialise(incoming_datagram.payload)) { onRemoteControlInputs(msg); return; }
            }
        }   
    }

    void onRemoteControlInputs(const dragonfly_msgs::msg::RemoteControlInputs& msg)
    {
        _control_loop.inputs().remote_control.update(msg);
    }

    void onBackupImuAttitude(const dragonfly_msgs::msg::Attitude& msg)
    {
        _control_loop.inputs().backup_attitude.update(msg);
    }

    void onPrimaryImuAttitude(const dragonfly_msgs::msg::Attitude& msg)
    {
        _control_loop.inputs().primary_attitude.update(msg);
    }

    HardwareInterface _hw_interface;
    dragonfly_datagram_protocol::Multiplexer<
        dragonfly_datagram_protocol::Node::FLIGHT_CONTROLLER,
        dragonfly_datagram_protocol::Node::PAYLOAD_COMPUTER,
        dragonfly_datagram_protocol::Node::REMOTE_CONTROL,
        dragonfly_datagram_protocol::Node::GROUND_STATION> _comms_multiplexer;
    ControlLoop _control_loop;
    ImuDriver _imu_driver;

    Timer _control_loop_timer;
};