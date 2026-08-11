Safety Considerations
========================

----

System Architecture
------------------------

The system architecture can be split in three sections : 

The **High-Criticality Section** contains the Flight Controller and the actuators.
It can handle automatic emergency procedures when the other sections are not available (see :ref:`degraded_control_modes`) 

The **Medium-Criticality Section** contains the Remote Control and the RF modem.
It can provide manual control when in RF range, and automatic emergency procedures otherwise (see :ref:`degraded_control_modes`) 

The **Low-Criticality Section** contains the rest of the system, it allows for nominal operations.

----

Failure scenarios
------------------------

Not all failure scenarios combinations are described here, but this section describes the behavior of the system for some "expected" failures, as well as a criticity score :

- 🟩 DragonFly can be manually controlled and landed safely
- 🟧 DragonFly can not be manually controlled and will try to wait until the situation improves, or attempt an automatic emergency landing if needed. Its safety and its surrounding's are not guaranteed.
- 🟥 DragonFly loses control of its actuators, it will crash soon 

**Summary**

==================================  ============
Failure                             Criticity
==================================  ============
Servos Power Outage                 🟥
Computing Compartment Power Outage  🟥
SensorsCompartment Power Outage     🟧
Payloads Computer Crash             🟧 / 🟩 depending on distance
==================================  ============

Payloads Computer Crash
_________________________

**Criticity :** 🟩 / 🟧 depending on distance

**Causes :** Firmware / Linux crash, power outage... 

**Consequences :** 

- Videostreams are no longer available

- The Flight Controller loses access to :
    
    - Gps Data
    - Altitude Data
    - 4G Signal
    
When in RF range, stabilised manual control can be taken and DragonFly can be landed safely.

When out of RF range, the Flight Controller will go into :ref:`closed_loop_descent` mode until

- RF signal is available again 
- The Payloads Computer restarts and 4G signal is available again
- It hits the ground

.. note:: The out of RF range behavior could be vastly improved by giving the Flight Controller direct access to GPS and Altitude data.
    This would allow for a "Return Home" behavior or emergency landing.

Servos Power Outage
_________________________

**Criticity :** 🟥

**Causes :** UBEC fault, bad wiring...

**Consequences :** DragonFly has no control over its attitude and most likely crashes very fast.

Computing Section Power Outage
_________________________

**Criticity :** 🟥

**Causes :** UBEC fault, bad wiring...

**Consequences :** The flight controller dies, so the servos no longer receive commands. 
Dragonfly crashes more or less quicly depending on servos behavior (return to 0 / keep previous position / go numb)

Sensors Section Power Outage
_________________________

**Criticity :** 🟧

**Causes :** UBEC fault, bad wiring...

**Consequences :** 

- The Flight Controller loses access to :
    
    - Gps Data
    - Altitude Data
    - 4G and RF signal

The Flight Controller goes into :ref:`closed_loop_descent` mode until

- Power comes back
- It hits the ground

.. note:: This behavior could be improved by giving the Flight Controller direct access to GPS and Altitude data.
    This would allow for a "Return Home" behavior or emergency landing.

----

Software
------------------------

Critical Section Software
_________________________

The software that runs in the **High** and **Medium** Criticality Sections (FC and RC) :

- Uses no dynamic allocations
- Is well unit tested
- Is monitored by a hardware watchdog