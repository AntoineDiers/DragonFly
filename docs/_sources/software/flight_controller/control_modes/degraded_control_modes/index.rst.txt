.. _degraded_control_modes:

Degraded Control Modes
------------------------------

Nominal Control Modes have required inputs, when one of these required inputs is missing, a Degraded Control Mode is activated depending on available inputs

============    ========    ================    ================    ================
RC commands     IMU data    Safe Altitude       Battery Level       Degraded Mode
============    ========    ================    ================    ================
✅              ✅                                                  CLOSED_LOOP_MANUAL
✅              ❌                                                  OPEN_LOOP_MANUAL
❌              ✅          ✅                  ✅                  CLOSED_LOOP_ORBIT
❌              ✅          ✅                  ❌ / ❓             CLOSED_LOOP_DESCENT
❌              ✅          ❌                                      CLOSED_LOOP_LANDING
❌              ✅          ❓                                      CLOSED_LOOP_DESCENT
❌              ❌          ✅                  ✅                  OPEN_LOOP_GLIDE
❌              ❌          ✅                  ❌ / ❓             OPEN_LOOP_DESCENT
❌              ❌          ❌                                      OPEN_LOOP_LANDING
❌              ❌          ❓                                      OPEN_LOOP_DESCENT
============    ========    ================    ================    ================

The outputs of the Degraded Modes are described below

----

CLOSED_LOOP_MANUAL
_______________________________

Same as :ref:`manual_mode`

OPEN_LOOP_MANUAL
_______________________________

Same as :ref:`manual_mode`, but **Roll Stabilization Level** and **Pitch Stabilization Level** are set to 0

CLOSED_LOOP_ORBIT
_______________________________

============    ============
Actuator        Control Mode
============    ============
Throttle        Open loop (40%)
Flaps           Open loop (0°)
Ailerons        Closed loop (roll setpoint = 10°) 
Elevator        Closed loop (altitude setpoint = 50m)
Rudder          Open loop (10°)
============    ============

.. _closed_loop_descent:

CLOSED_LOOP_DESCENT
_______________________________

============    ============
Actuator        Control Mode
============    ============
Throttle        Open loop (30%)
Flaps           Open loop (0°)
Ailerons        Closed loop (roll setpoint = 0°) 
Elevator        Closed loop (pitch setpoint = 5°)
Rudder          Open loop (0°)
============    ============

CLOSED_LOOP_LANDING
_______________________________

============    ============
Actuator        Control Mode
============    ============
Throttle        Open loop (30% -> 0% depending on altitude)
Flaps           Open loop (0°)
Ailerons        Closed loop (roll setpoint = 0°) 
Elevator        Closed loop (pitch setpoint = 5°)
Rudder          Open loop (0°)
============    ============

OPEN_LOOP_GLIDE
_______________________________

============    ============
Actuator        Control Mode
============    ============
Throttle        Open loop (40%)
Flaps           Open loop (0°)
Ailerons        Open loop (0°)
Elevator        Open loop (0°)
Rudder          Open loop (0°)
============    ============

OPEN_LOOP_DESCENT
_______________________________

============    ============
Actuator        Control Mode
============    ============
Throttle        Open loop (30%)
Flaps           Open loop (0°)
Ailerons        Open loop (0°)
Elevator        Open loop (5°)
Rudder          Open loop (0°)
============    ============

OPEN_LOOP_LANDING
_______________________________

============    ============
Actuator        Control Mode
============    ============
Throttle        Open loop (30% -> 0% depending on altitude)
Flaps           Open loop (20°)
Ailerons        Open loop (0°)
Elevator        Open loop (5°)
Rudder          Open loop (0°)
============    ============