#pragma once

#include <cstdint>

struct ActuatorCommands
{
    float throttle_percent;
    float flaps_deg;
    float ailerons_deg;
    float rudder_deg;
    float elevator_deg;
};