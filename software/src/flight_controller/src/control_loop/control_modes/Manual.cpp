#include "Manual.h"
#include <config/flight_controller/control_loop/control_modes/Config.h>
#include <config/flight_controller/actuators/limits/Config.h>

#include "../Utils.h"

using namespace config::flight_controller::control_loop::control_modes::manual;
using namespace config::flight_controller::actuators::limits;

dragonfly_msgs::msg::ActuatorCommands control_modes::manual::tick(
    const dragonfly_msgs::msg::remote_control::Inputs& remote_control_inputs, 
    const FlightControllerInputs::Attitude& attitude, 
    Pids &pids)
{
    float left_right = centeredRcInputToFloat(remote_control_inputs.left_right);
    float down_up = centeredRcInputToFloat(remote_control_inputs.down_up);
    float throttle = rcInputToFloat(remote_control_inputs.throttle);
    float flaps = rcInputToFloat(remote_control_inputs.flaps);

    float roll_setpoint_deg = - left_right * MAX_ROLL_SETPOINT_DEG;
    float ailerons_setpoint_deg = - left_right * MAX_AILERON_ANGLE_DEG;
    float roll_stabilization_level = rcInputToFloat(remote_control_inputs.roll_stabilization_level);
    float ailerons_cmd_deg = roll_stabilization_level * pids.roll.tick(roll_setpoint_deg, attitude.roll_deg) + (1.0f - roll_stabilization_level) * ailerons_setpoint_deg;

    float pitch_setpoint_deg = down_up * MAX_PITCH_SETPOINT_DEG;
    float elevator_setpoint_deg = down_up * MAX_AILERON_ANGLE_DEG;
    float pitch_stabilization_level = rcInputToFloat(remote_control_inputs.pitch_stabilization_level);
    float elevator_cmd_deg = pitch_stabilization_level * pids.pitch.tick(pitch_setpoint_deg, attitude.pitch_deg) + (1.0f - pitch_stabilization_level) * elevator_setpoint_deg;

    float rudder_cmd_deg = -remote_control_inputs.left_right * MAX_RUDDER_ANGLE_DEG;

    dragonfly_msgs::msg::ActuatorCommands res;
    res.throttle_percent = 100 * throttle;
    res.flaps_deg = flaps * config::flight_controller::actuators::limits::MAX_FLAP_ANGLE_DEG;
    res.ailerons_deg = ailerons_cmd_deg;
    res.elevator_deg = elevator_cmd_deg;
    res.rudder_deg = rudder_cmd_deg;
    return res;
}