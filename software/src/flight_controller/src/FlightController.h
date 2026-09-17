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
#include <config/flight_controller/airspeed_sensor/Config.h>

#include "imu_driver/ImuDriver.h"
#include "airspeed_sensor_driver/AirspeedSensorDriver.h"
#include "battery_sensors_driver/BatterySensorsDriver.h"
#include "control_loop/ControlLoop.h"
#include "hardware/HardwareInterface.h"
#include "pressure_sensor_driver/PressureSensorDriver.h"

#include "events/Events.h"

#include <rpi_pico_utils/timer/Timer.h>
#include <dragonfly_datagram_protocol/Multiplexer.h>

class FlightController
{
public:

    FlightController(HardwareInterface hw_interface) : 
        _events(hw_interface.clock),
        _inputs(hw_interface.clock),
        _hw_interface(hw_interface),
        _comms_multiplexer(hw_interface.clock, hw_interface.rf_uart, hw_interface.pc_spi, PeerId::FLIGHT_CONTROLLER, _events.getMultiplexerEvents()),
        _control_loop(&_inputs, hw_interface.clock),
        _primary_imu_driver(config::flight_controller::imu::IMU_DRIVER, &_inputs.primary_attitude, hw_interface.clock, hw_interface.primary_imu_i2c, hw_interface.primary_imu_int_pin, hw_interface.primary_imu_rst_pin),
        _backup_imu_driver(config::flight_controller::imu::IMU_DRIVER, &_inputs.backup_attitude, hw_interface.clock, hw_interface.backup_imu_i2c, hw_interface.backup_imu_int_pin, hw_interface.backup_imu_rst_pin),
        _airspeed_sensor_driver(config::flight_controller::airspeed_sensor::CONFIG, hw_interface.clock, hw_interface.airspeed_sensor_i2c, &_inputs.airspeed_km_h),
        _battery_sensors_driver(config::flight_controller::battery::DRIVER_CONFIG, &_inputs, hw_interface.clock, hw_interface.ads1115_i2c),
        _pressure_sensor_driver(_hw_interface.clock, hw_interface.pressure_sensor_i2c, hw_interface.pressure_sensor_int_pin, _inputs.pressure_Pa),
        _control_loop_timer(hw_interface.clock, config::flight_controller::control_loop::PERIOD_MS),
        _system_led_timer(hw_interface.clock, config::flight_controller::led::SYSTEM_LED_PERIOD_MS),
        _send_state_overview_timer(hw_interface.clock, config::flight_controller::comms::SEND_STATE_OVERVIEW_PERIOD_MS),
        _send_data_rates_timer(hw_interface.clock, config::flight_controller::comms::SEND_DATA_RATES_PERIOD_MS) {}

    void tick()
    {
        // Poll and handle incoming messages
        dragonfly_datagram_protocol::Multiplexer::DatagramPair incoming_datagrams = _comms_multiplexer.poll();
        if(incoming_datagrams.local_link_datagram) { onMessage(incoming_datagrams.local_link_datagram.value()); }
        if(incoming_datagrams.remote_link_datagram) { onMessage(incoming_datagrams.remote_link_datagram.value()); }

        // tick drivers
        _battery_sensors_driver.tick();
        _airspeed_sensor_driver.tick();
        _primary_imu_driver.tick();
        _backup_imu_driver.tick();
        _pressure_sensor_driver.tick();

        // Execute periodic actions
        if(_system_led_timer.poll()) { updateSystemLed(); }
        if(_control_loop_timer.poll()) { sendActuatorCommands(_control_loop.tick()); }
        if(_send_state_overview_timer.poll()) { sendStateOverview(); }
        if(_send_data_rates_timer.poll()) { sendDataRates(); }
    }

private:

    void updateSystemLed()
    {
        _system_led_state = !_system_led_state;
        _hw_interface.system_led->set(_system_led_state);
    }

    void sendDataRates()
    {
        auto msg = _comms_multiplexer.getDataRates();
        std::cout << msg._serialiseJson().dump(4) << "\n";
        _comms_multiplexer.sendMessage(msg, PeerId::GROUND_COMPUTER);
    }

    void sendStateOverview()
    {
        dragonfly_msgs::msgs::flight_controller::StateOverview state;

        dragonfly_msgs::enums::DiagLevel primary_imu_diag_level = _primary_imu_driver.getDiagLevel();
        dragonfly_msgs::enums::DiagLevel backup_imu_diag_level = _backup_imu_driver.getDiagLevel();
        state.input_diags.attitude.set(
            (primary_imu_diag_level != dragonfly_msgs::enums::DiagLevel::OK && backup_imu_diag_level != dragonfly_msgs::enums::DiagLevel::OK) ? dragonfly_msgs::enums::DiagLevel::ERR : 
            (primary_imu_diag_level == dragonfly_msgs::enums::DiagLevel::OK && backup_imu_diag_level == dragonfly_msgs::enums::DiagLevel::OK ? dragonfly_msgs::enums::DiagLevel::OK : 
            dragonfly_msgs::enums::DiagLevel::WARN));

        state.input_diags.airspeed.set(_airspeed_sensor_driver.getDiagLevel());
        state.input_diags.battery_level.set(_battery_sensors_driver.getDiagLevel());
        state.input_diags.remote_control.set(_inputs.remote_control.getState().is_ok ? dragonfly_msgs::enums::DiagLevel::OK : dragonfly_msgs::enums::DiagLevel::ERR);

        _comms_multiplexer.sendMessage(state, PeerId::GROUND_COMPUTER);
    }

    void sendActuatorCommands(const ActuatorCommands& commands)
    {
        // Replace the ESC dead zone with our own
        float throttle_percent = commands.throttle_percent < config::flight_controller::actuators::esc::DEAD_ZONE_PERCENT ? 0 : 
            config::flight_controller::actuators::esc::MIN_THROTTLE_PERCENT + (commands.throttle_percent - config::flight_controller::actuators::esc::DEAD_ZONE_PERCENT) * 
            (100.0f - config::flight_controller::actuators::esc::MIN_THROTTLE_PERCENT) / (100.0f - config::flight_controller::actuators::esc::DEAD_ZONE_PERCENT);

        // Compute flaperons angles from flaps and ailerons commands
        float left_flaperon_cmd = commands.flaps_deg + commands.ailerons_deg;
        float right_flaperon_cmd = commands.flaps_deg - commands.ailerons_deg;

        // Compute ruddervator angles from rudder and elevators commands
        float left_ruddervator_cmd = commands.rudder_deg + commands.elevator_deg;
        float right_ruddervator_cmd = commands.rudder_deg - commands.elevator_deg;

        throttle_percent =          std::clamp(throttle_percent,            0.0f,                                                                       config::flight_controller::actuators::limits::MAX_THROTTLE_PERCENT);
        left_flaperon_cmd =         std::clamp(left_flaperon_cmd,           -config::flight_controller::actuators::limits::MAX_FLAPERON_ANGLE_DEG,      config::flight_controller::actuators::limits::MAX_FLAPERON_ANGLE_DEG);
        right_flaperon_cmd =        std::clamp(right_flaperon_cmd,          -config::flight_controller::actuators::limits::MAX_FLAPERON_ANGLE_DEG,      config::flight_controller::actuators::limits::MAX_FLAPERON_ANGLE_DEG);
        left_ruddervator_cmd =      std::clamp(left_ruddervator_cmd,        -config::flight_controller::actuators::limits::MAX_RUDDERVATOR_ANGLE_DEG,   config::flight_controller::actuators::limits::MAX_RUDDERVATOR_ANGLE_DEG);
        right_ruddervator_cmd =     std::clamp(right_ruddervator_cmd,       -config::flight_controller::actuators::limits::MAX_RUDDERVATOR_ANGLE_DEG,   config::flight_controller::actuators::limits::MAX_RUDDERVATOR_ANGLE_DEG);
    
        _hw_interface.esc_pwm->setCommandPercent(throttle_percent);
        sendServoCommand(_hw_interface.left_flaperon_pwm, left_flaperon_cmd);
        sendServoCommand(_hw_interface.right_flaperon_pwm, right_flaperon_cmd);
        sendServoCommand(_hw_interface.left_ruddervator_pwm, left_ruddervator_cmd);
        sendServoCommand(_hw_interface.right_ruddervator_pwm, right_ruddervator_cmd);
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
                dragonfly_msgs::msgs::remote_control::Inputs msg;
                if(msg._deserialise(incoming_datagram.data)) { onRemoteControlInputs(msg); return; }
            }
        }

        if(incoming_datagram.dst == PeerId::FLIGHT_CONTROLLER)
        {
            _events.unhandled_datagram.trigger();
        }
    }

    void onRemoteControlInputs(const dragonfly_msgs::msgs::remote_control::Inputs& msg)
    {
        _inputs.remote_control.update(msg);
    }

    FlightControllerEvents _events;

    FlightControllerInputs _inputs;
    HardwareInterface _hw_interface;

    dragonfly_datagram_protocol::Multiplexer _comms_multiplexer;
    ControlLoop _control_loop;
    ImuDriver _primary_imu_driver;
    ImuDriver _backup_imu_driver;
    AirspeedSensorDriver _airspeed_sensor_driver;
    BatterySensorsDriver _battery_sensors_driver;
    PressureSensorDriver _pressure_sensor_driver;

    Timer _control_loop_timer;
    Timer _system_led_timer;
    Timer _send_state_overview_timer;
    Timer _send_data_rates_timer;

    bool _system_led_state = false;
};