#pragma once

#include <rpi_pico_utils/utils/Periodic.h>
#include <dragonfly_msgs/msgs/remote_control/Inputs.h>

#include <config/flight_controller/inputs/Config.h>

class FlightControllerInputs
{
public:

    struct Attitude
    {
        float roll_deg;
        float pitch_deg;
    };

    struct BatteryLevel
    {
        float level_percent;
        float level_std_dev;
    };

    FlightControllerInputs(ClockInterface* clock) : 
        remote_control          (config::flight_controller::inputs::REMOTE_CONTROL_RATE,        clock),
        primary_attitude        (config::flight_controller::inputs::PRIMARY_ATTITUDE_RATE,      clock),
        backup_attitude         (config::flight_controller::inputs::BACKUP_ATTITUDE_RATE,       clock),
        altitude_m              (config::flight_controller::inputs::ALTITUDE_RATE,              clock),
        airspeed_km_h           (config::flight_controller::inputs::AIRSPEED_RATE,              clock),
        battery_level           (config::flight_controller::inputs::BATTERY_LEVEL_RATE,         clock),
        battery_voltage_V       (config::flight_controller::inputs::ADS1115_DATA_RATE,          clock),
        battery_current_A       (config::flight_controller::inputs::ADS1115_DATA_RATE,          clock),
        current_after_esc_A     (config::flight_controller::inputs::ADS1115_DATA_RATE,          clock),
        current_after_servos_A  (config::flight_controller::inputs::ADS1115_DATA_RATE,          clock),
        pressure_Pa             (config::flight_controller::inputs::PRESSURE_DATA_RATE,         clock)
        {}

    Periodic<dragonfly_msgs::msgs::remote_control::Inputs> remote_control;
    Periodic<Attitude> primary_attitude;
    Periodic<Attitude> backup_attitude;
    Periodic<float> altitude_m;
    Periodic<float> airspeed_km_h;
    Periodic<BatteryLevel> battery_level;
    Periodic<float> battery_voltage_V;
    Periodic<float> battery_current_A;
    Periodic<float> current_after_esc_A;
    Periodic<float> current_after_servos_A;
    Periodic<float> pressure_Pa;
};
