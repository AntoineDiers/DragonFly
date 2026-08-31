Nominal Control Modes
-------------------------------

IDLE
_______________________________

Required Inputs
###############################

None

Outputs
###############################

======================  ==========================================
Actuator                Control Mode                                    
======================  ==========================================
Throttle                0
Flaps                   0
Ailerons                0
Elevator                0
Rudder                  0
======================  ==========================================

----

.. _manual_mode:

MANUAL
_______________________________

Required Inputs
###############################

- IMU Data
- Remote Control Inputs

Outputs
###############################

Throttle and servo commands are computed using a mix of Remote Control data and active stabilisation :

======================  ==========================================
Actuator                Control Mode                                    
======================  ==========================================
Throttle                Open Loop (Throttle command from RC)
Flaps                   Open Loop (Flaps command from RC)
Ailerons                Open/Closed Loop mix (linearly depends on Roll Stabilization Level command from RC)
Elevator                Open/Closed Loop mix (linearly depends on Pitch Stabilization Level command from RC)
Rudder                  Open Loop (Left/Right command from RC)
======================  ==========================================

----

AUTO
_______________________________

.. note::
    Not implemented yet