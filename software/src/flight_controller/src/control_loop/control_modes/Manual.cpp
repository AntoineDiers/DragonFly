#include "Manual.h"
#include <config/flight_controller/control_loop/control_modes/Config.h>
#include <config/flight_controller/actuators/limits/Config.h>

#include "../ActuatorCommands.h"

using namespace config::flight_controller::control_loop::control_modes::manual;
using namespace config::flight_controller::actuators::limits;

ActuatorCommands control_modes::manual::tick(
    const dragonfly_msgs::msgs::remote_control::Inputs& remote_control_inputs, 
    const FlightControllerInputs::Attitude& attitude, 
    Pids &pids)
{
    float roll_setpoint_deg = - remote_control_inputs.left_right.get() * MAX_ROLL_SETPOINT_DEG;
    float ailerons_setpoint_deg = - remote_control_inputs.left_right.get() * MAX_AILERON_ANGLE_DEG;
    float ailerons_cmd_deg =    (remote_control_inputs.roll_stab_level_percent.get() / 100) * 
                                pids.roll.tick(roll_setpoint_deg, attitude.roll_deg) + 
                                (1.0f - (remote_control_inputs.roll_stab_level_percent.get() / 100)) * 
                                ailerons_setpoint_deg;

    float pitch_setpoint_deg = remote_control_inputs.down_up.get() * MAX_PITCH_SETPOINT_DEG;
    float elevator_setpoint_deg = remote_control_inputs.down_up.get() * MAX_AILERON_ANGLE_DEG;
    float elevator_cmd_deg =    (remote_control_inputs.pitch_stab_level_percent.get() / 100) * 
                                pids.pitch.tick(pitch_setpoint_deg, attitude.pitch_deg) + 
                                (1.0f - (remote_control_inputs.pitch_stab_level_percent.get() / 100)) * 
                                elevator_setpoint_deg;

    float rudder_cmd_deg = -remote_control_inputs.left_right.get() * MAX_RUDDER_ANGLE_DEG;

    ActuatorCommands res;
    res.throttle_percent = remote_control_inputs.throttle_percent.get();
    res.flaps_deg = remote_control_inputs.flaps_deg.get();
    res.ailerons_deg = ailerons_cmd_deg;
    res.elevator_deg = elevator_cmd_deg;
    res.rudder_deg = rudder_cmd_deg;
    return res;
}