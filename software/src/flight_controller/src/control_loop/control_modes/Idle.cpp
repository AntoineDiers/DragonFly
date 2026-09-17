#include "Idle.h"

ActuatorCommands control_modes::idle::tick()
{
    ActuatorCommands res;
    res.throttle_percent = 0;
    res.flaps_deg = 0;
    res.ailerons_deg = 0;
    res.elevator_deg = 0;
    res.rudder_deg = 0;
    return res;
}