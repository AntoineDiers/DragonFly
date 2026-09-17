#pragma once

#include <dragonfly_utils/common_structs/StateMachine.h>

#include "../inputs/FlightControllerInputs.h"

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

    ControlLoop(FlightControllerInputs* inputs, ClockInterface* clock) :
        _clock(clock),
        _inputs(inputs),
        _pids(clock) {}
        
    ActuatorCommands tick()
    {
        // Get inputs
        
        auto remote_control_inputs =    _inputs->remote_control.getLastValue(config::flight_controller::control_loop::REMOTE_CONTROL_TIMEOUT_MS);
        auto attitude =                 _inputs->primary_attitude.getLastValue(config::flight_controller::control_loop::ATTITUDE_TIMEOUT_MS);
        if(!attitude) { attitude =      _inputs->backup_attitude.getLastValue(config::flight_controller::control_loop::ATTITUDE_TIMEOUT_MS); }
        auto altitude_m =                 _inputs->altitude_m.getLastValue(config::flight_controller::control_loop::ALTITUDE_TIMEOUT_MS);
        auto battery_level =            _inputs->battery_level.getLastValue(config::flight_controller::control_loop::BATTERY_LEVEL_TIMEOUT);

        // Update Nominal Control Mode according to remote control inputs
        if(remote_control_inputs)
        {
            _state.nominal_control_mode = remote_control_inputs->arm_enabled.get() ? 
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
                        remote_control_inputs.value(), 
                        attitude.value(),
                        _pids);
                }
                break;
            }
            case ControlModes::AUTO:
            {
                // Not implemented yet
            }
        }

        // Activate a degraded control mode if a required input is missing
        if(remote_control_inputs)
        {
            if(attitude)
            {
                return fallback_modes::closed_loop_manual::tick(
                    remote_control_inputs.value(), 
                    attitude.value(), 
                    _pids);
            }
            else
            {
                return fallback_modes::open_loop_manual::tick(
                    remote_control_inputs.value());
            }
        }
        else
        {
            if(attitude)
            {
                if(altitude_m)
                {
                    if(altitude_m.value() > config::flight_controller::control_loop::fallback_modes::SAFE_ALTITDE_LIMIT_M)
                    {
                        if(battery_level && battery_level.value().level_percent > config::flight_controller::control_loop::fallback_modes::SAFE_BATTERY_LEVEL_PERCENT)
                        {
                            return fallback_modes::closed_loop_orbit::tick(
                                attitude.value(), 
                                altitude_m.value(), 
                                _pids);
                        }
                        else
                        {
                            return fallback_modes::closed_loop_descent::tick(
                                attitude.value(), 
                                _pids);
                        }
                    }
                    else
                    {
                        return fallback_modes::closed_loop_landing::tick(
                            attitude.value(), 
                            altitude_m.value(), 
                            _pids);
                    }
                }
                else
                {
                    return fallback_modes::closed_loop_descent::tick(
                        attitude.value(), 
                        _pids);
                }
            }
            else
            {
                if(altitude_m)
                {
                    if(altitude_m.value() > config::flight_controller::control_loop::fallback_modes::SAFE_ALTITDE_LIMIT_M)
                    {
                        if(battery_level && battery_level.value().level_percent > config::flight_controller::control_loop::fallback_modes::SAFE_BATTERY_LEVEL_PERCENT)
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
                        return fallback_modes::open_loop_landing::tick(altitude_m.value());
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

    ClockInterface* _clock;
    FlightControllerInputs* _inputs;
    Pids _pids;

    State _state;
};