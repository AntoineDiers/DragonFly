#include "ClosedLoopOrbit.h"
#include <config/flight_controller/control_loop/fallback_modes/Config.h>

using namespace config::flight_controller::control_loop::fallback_modes::closed_loop_orbit;

ActuatorCommands fallback_modes::closed_loop_orbit::tick(
    const FlightControllerInputs::Attitude &attitude, 
    const float &altitude, 
    Pids &pids)
{
    float pitch_setpoint = pids.altitude.tick(ALTITUDE_SETPOINT_M, altitude);
    pitch_setpoint = std::clamp(pitch_setpoint, -MAX_PITCH_SETPOINT_DEG, MAX_PITCH_SETPOINT_DEG);

    ActuatorCommands res;
    res.throttle_percent = THROTTLE_PERCENT;
    res.flaps_deg = FLAPS_DEG;
    res.ailerons_deg = pids.roll.tick(ROLL_SETPOINT_DEG, attitude.roll_deg);
    res.elevator_deg = pids.pitch.tick(pitch_setpoint, attitude.pitch_deg);
    res.rudder_deg = RUDDER_DEG;
    return res;
}