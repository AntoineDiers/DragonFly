Control Modes
-------------------------------

Inputs/Outputs
===============================

Inputs
_______________________________

======================  ==========================================
Input                   Origin                                    
======================  ==========================================
Attitude (roll/pitch)   Main IMU                                  
.                       Backup IMU (via Payload Computer)         
Altitude                Backup IMU (via Payload Computer)           
Remote Control Inputs   RF Communication                          
.                       4G Communication (via Payload Computer)   
======================  ==========================================

Remote Control Inputs contain the following :

==========================  ==========================================
Input                       Description                                    
==========================  ==========================================
Armed switch                The Flight Controller is in IDLE mode until this switch is turned on
Auto switch                 A switch between AUTO and MANUAL mode (unused for now)
Throttle                    The Throttle command between 0 and 1
Left/Right                  The Turn command between -1 and 1, -1 being left
Down/Up                     The Pitch command between -1 and 1, -1 being pitch up
Roll Stabilization Level    The level of Roll Stabilization between 0 and 1, 0 being no active stabilization
Pitch Stabilization Level   The level of Pitch Stabilization between 0 and 1, 0 being no active stabilization
Flaps Angle                 The mean angle of the flaperons between 0 and 45
==========================  ==========================================

Outputs
_______________________________

- Throttle
- Servos commands

    - Flaps (mean angle of both flaperons)
    - Ailerons (angle difference between flaperons)
    - Elevator
    - Rudder


.. toctree::
    :maxdepth: 2

    nominal_control_modes/index
    degraded_control_modes/index


