Aerodynamics
=================

Aerodynamics simulations were made using the free version of `SimFlow <https://sim-flow.com/>`__. The simulation data can be found in the **hardware/CFD** folder.

Constants
-----------------

- **Weight** : 5kg
- **Degraded Mode Speed** : 50km/h
- **Main Wing mounting angle** : 4°

Stability
-----------------

When we lose attitude data, Autoplane enters a degraded mode where all servos are set to neutral position, and speed is set to **Degraded Mode Speed**.

The plane is designed to be stable in those conditions, meaning that the stabiliser should keep the pitch at an angle where enough lift is generated to compensate for the weight

To ensure this : 

- **Step 1** : Compute **Degraded Mode Pitch** (the pitch angle at whitch lift compensates weight at **Degraded Mode Speed**)

    - Result : -2.5°

- **Step 2** : Compute the **Stabiliser Mounting Angle** (the stabiliser mounting angle at which My = 0 at **Degraded Mode Pitch** and **Degraded Mode Speed**)

    - Result : -2°

- **Step 3** : Verify that the equilibrium is stable (dMy/dPitch < 0 at **Degraded Mode Pitch** and **Degraded Mode Speed**)

    - Result : Ok ✅

