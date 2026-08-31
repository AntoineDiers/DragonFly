#pragma once

#include <algorithm>
#include <iostream>
#include <math.h>

#include <config/flight_controller/actuators/Config.h>
#include <config/flight_controller/actuators/limits/Config.h>
#include <config/flight_controller/comms/Config.h>
#include <config/flight_controller/control_loop/Config.h>
#include <config/flight_controller/imu/Config.h>
#include <config/flight_controller/led/Config.h>
#include <config/flight_controller/battery/Config.h>

#include "imu_driver/ImuDriver.h"
#include "battery_stats_handler/BatteryStatsHandler.h"
#include "control_loop/ControlLoop.h"
#include "hardware/HardwareInterface.h"

#include <rpi_pico_utils/timer/Timer.h>
#include <dragonfly_datagram_protocol/Multiplexer.h>

class FlightController
{
public:

    FlightController(HardwareInterface hw_interface) : 
        _inputs(hw_interface.clock),
        _hw_interface(hw_interface),
        _comms_multiplexer(hw_interface.clock, hw_interface.pc_uart, hw_interface.rf_uart, PeerId::FLIGHT_CONTROLLER),
        _control_loop(&_inputs, hw_interface.clock),
        _primary_imu_driver(config::flight_controller::imu::IMU_DRIVER, &_inputs.primary_attitude, hw_interface.clock, hw_interface.primary_imu_i2c, hw_interface.primary_imu_int_pin, hw_interface.primary_imu_rst_pin),
        _backup_imu_driver(config::flight_controller::imu::IMU_DRIVER, &_inputs.backup_attitude, hw_interface.clock, hw_interface.backup_imu_i2c, hw_interface.backup_imu_int_pin, hw_interface.backup_imu_rst_pin),
        _battery_stats_handler(config::flight_controller::battery::STATS_HANDLER_CONFIG, &_inputs, hw_interface.clock, hw_interface.ads1115_i2c),
        _control_loop_timer(hw_interface.clock, config::flight_controller::control_loop::PERIOD_MS),
        _system_led_timer(hw_interface.clock, config::flight_controller::led::SYSTEM_LED_PERIOD_MS) {}

    void tick()
    {
        // Update system led
        if(_system_led_timer.poll())
        {
            _system_led_state = !_system_led_state;
            _hw_interface.system_led->set(_system_led_state);
        }

        // Update battery stats handler
        _battery_stats_handler.tick();

        // Poll and handle incoming messages
        dragonfly_datagram_protocol::Multiplexer::DatagramPair incoming_datagrams = _comms_multiplexer.poll();
        if(incoming_datagrams.local_link_datagram) { onMessage(incoming_datagrams.local_link_datagram.value()); }
        if(incoming_datagrams.remote_link_datagram) { onMessage(incoming_datagrams.remote_link_datagram.value()); }

        // Poll IMU data
        _primary_imu_driver.poll();
        _backup_imu_driver.poll();

        // Run state machine every config::flight_controller::CONTROL_PERIOD_US
        if(_control_loop_timer.poll())
        {

            sendActuatorCommands(_control_loop.tick());

            /*dragonfly_msgs::msg::FlightControllerInputStates input_states = _control_loop.inputs().getState();
            
            std::cout << "Input States : " << "\n";
            std::cout << " - remote_control :   " << (int)input_states.remote_control.state << " " << input_states.remote_control.rate_hz << " Hz " << input_states.remote_control.time_since_last_ms << " ms\n";
            std::cout << " - primary_attitude : " << (int)input_states.primary_attitude.state << " " << input_states.primary_attitude.rate_hz << " Hz " << input_states.primary_attitude.time_since_last_ms << " ms\n";
            std::cout << " - backup_attitude :  " << (int)input_states.backup_attitude.state << " " << input_states.backup_attitude.rate_hz << " Hz " << input_states.backup_attitude.time_since_last_ms << " ms\n";
            std::cout << " - altitude :         " << (int)input_states.altitude.state << " " << input_states.altitude.rate_hz << " Hz " << input_states.altitude.time_since_last_ms << " ms\n";
            std::cout << " - battery_level :    " << (int)input_states.battery_level.state << " " << input_states.battery_level.rate_hz << " Hz " << input_states.battery_level.time_since_last_ms << " ms\n";
            */
        
        }
    }

private:

    void sendActuatorCommands(dragonfly_msgs::msg::ActuatorCommands commands)
    {
        // Replace the ESC dead zone with our own
        float throttle_percent = commands.throttle_percent < config::flight_controller::actuators::esc::DEAD_ZONE_PERCENT ? 0 : 
            config::flight_controller::actuators::esc::MIN_THROTTLE_PERCENT + (commands.throttle_percent - config::flight_controller::actuators::esc::DEAD_ZONE_PERCENT) * 
            (100.0f - config::flight_controller::actuators::esc::MIN_THROTTLE_PERCENT) / (100.0f - config::flight_controller::actuators::esc::DEAD_ZONE_PERCENT);

        // Compute flaperons angles from flaps and ailerons commands
        float left_flaperon_cmd = commands.flaps_deg + commands.ailerons_deg;
        float right_flaperon_cmd = commands.flaps_deg - commands.ailerons_deg;

        throttle_percent =          std::clamp(throttle_percent,            0.0f,                                                                  config::flight_controller::actuators::limits::MAX_THROTTLE_PERCENT);
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
        std::clamp(angle_deg, -90.0f ,90.0f);
        pwm->setCommandPercent(50 + 50 * (angle_deg / 90));
    }

    void onMessage(const dragonfly_datagram_protocol::DatagramPacket& incoming_datagram)
    {
        if(incoming_datagram.src == PeerId::REMOTE_CONTROL && incoming_datagram.dst == PeerId::FLIGHT_CONTROLLER)
        {
            {
                dragonfly_msgs::msg::remote_control::Inputs msg;
                if(msg._deserialise(incoming_datagram.data)) { onRemoteControlInputs(msg); return; }
            }
        }   
    }

    void onRemoteControlInputs(const dragonfly_msgs::msg::remote_control::Inputs& msg)
    {
        msg._print();
        _inputs.remote_control.update(msg);
    }

    FlightControllerInputs _inputs;
    HardwareInterface _hw_interface;

    dragonfly_datagram_protocol::Multiplexer _comms_multiplexer;
    ControlLoop _control_loop;
    ImuDriver _primary_imu_driver;
    ImuDriver _backup_imu_driver;
    BatteryStatsHandler _battery_stats_handler;

    Timer _control_loop_timer;
    Timer _system_led_timer;

    bool _system_led_state = false;
};