#include "Idle.h"

dragonfly_msgs::msg::ActuatorCommands control_modes::idle::tick()
{
    dragonfly_msgs::msg::ActuatorCommands res;
    res.throttle_percent = 0;
    res.flaps_deg = 0;
    res.ailerons_deg = 0;
    res.elevator_deg = 0;
    res.rudder_deg = 0;
    return res;
}