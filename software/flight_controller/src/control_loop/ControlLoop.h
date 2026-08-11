#pragma once

#include <dragonfly_utils/common_structs/StateMachine.h>

#include "Inputs.h"

#include "control_modes/Idle.h"
#include "control_modes/Manual.h"

#include "fallback_modes/ClosedLoopDescent.h"
#include "fallback_modes/ClosedLoopLanding.h"
#include "fallback_modes/ClosedLoopManual.h"
#include "fallback_modes/ClosedLoopOrbit.h"
#include "fallback_modes/OpenLoopDescent.h"
#include "fallback_modes/OpenLoopGlide.h"
#include "fallback_modes/OpenLoopLanding.h"
#include "fallback_modes/OpenLoopManual.h"

#include <config/flight_controller/control_loop/fallback_modes/Config.h>

class ControlLoop
{
public:

    struct State
    {
        ControlModes nominal_control_mode = ControlModes::IDLE;
        FallbackModes fallback_mode = FallbackModes::NONE;
    };

    ControlLoop(ClockInterface* clock) :
        _inputs(clock),
        _pids(clock) {}

    control_loop::Inputs& inputs() { return _inputs; }

    dragonfly_msgs::msg::ActuatorCommands tick()
    {
        // Get inputs
        auto remote_control_inputs = _inputs.remote_control.getLast();
        auto attitude = _inputs.primary_attitude.getLast();
        if(!attitude) { attitude = _inputs.backup_attitude.getLast(); }
        auto altitude = _inputs.altitude.getLast();
        auto battery_level = _inputs.battery_level.getLast();

        // Update Nominal Control Mode according to remote control inputs
        if(remote_control_inputs)
        {
            _state.nominal_control_mode = remote_control_inputs->value.is_armed ? 
                ControlModes::MANUAL : 
                ControlModes::IDLE;
        }

        // Try to handle the current Nominal Control Mode
        _state.fallback_mode = FallbackModes::NONE;
        switch (_state.nominal_control_mode)
        {
            case ControlModes::IDLE:
            {
                return control_modes::idle::tick();
            }
            case ControlModes::MANUAL:
            {
                if(remote_control_inputs && attitude)
                {
                    return control_modes::manual::tick(
                        remote_control_inputs->value, 
                        attitude->value,
                        _pids);
                }
                break;
            }
        }

        // Activate a degraded control mode if a required input is missing
        if(remote_control_inputs)
        {
            if(attitude)
            {
                return fallback_modes::closed_loop_manual::tick(
                    remote_control_inputs->value, 
                    attitude->value, 
                    _pids);
            }
            else
            {
                return fallback_modes::open_loop_manual::tick(
                    remote_control_inputs->value);
            }
        }
        else
        {
            if(attitude)
            {
                if(altitude)
                {
                    if(altitude->value.altitude_m > config::flight_controller::control_loop::fallback_modes::SAFE_ALTITDE_LIMIT_M)
                    {
                        if(battery_level && battery_level->value.battery_level_percent > config::flight_controller::control_loop::fallback_modes::SAFE_BATTERY_LEVEL_PERCENT)
                        {
                            return fallback_modes::closed_loop_orbit::tick(
                                attitude->value, 
                                altitude->value, 
                                _pids);
                        }
                        else
                        {
                            return fallback_modes::closed_loop_descent::tick(
                                attitude->value, 
                                _pids);
                        }
                    }
                    else
                    {
                        return fallback_modes::closed_loop_landing::tick(
                            attitude->value, 
                            altitude->value, 
                            _pids);
                    }
                }
                else
                {
                    return fallback_modes::closed_loop_descent::tick(
                        attitude->value, 
                        _pids);
                }
            }
            else
            {
                if(altitude)
                {
                    if(altitude->value.altitude_m > config::flight_controller::control_loop::fallback_modes::SAFE_ALTITDE_LIMIT_M)
                    {
                        if(battery_level && battery_level->value.battery_level_percent > config::flight_controller::control_loop::fallback_modes::SAFE_BATTERY_LEVEL_PERCENT)
                        {
                            return fallback_modes::open_loop_glide::tick();
                        }
                        else
                        {
                            return fallback_modes::open_loop_descent::tick();
                        }
                    }
                    else
                    {
                        return fallback_modes::open_loop_landing::tick(altitude->value);
                    }
                }
                else
                {
                    return fallback_modes::open_loop_descent::tick();
                }
            }
        }
    }

private:

    control_loop::Inputs _inputs;
    Pids _pids;

    State _state;
};