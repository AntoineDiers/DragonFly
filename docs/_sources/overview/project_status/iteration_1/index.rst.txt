DragonFly Mk1
=============

Goals
_____

Create a basic prototype to de-risk the most critical aspects early : 

- Mechanical strength
- Electronics design
- Challenging software devs ( low latency videostreaming )


Outcome
_______

- Mechanical Design
    - My first mechanical design was way too weak. I overestimated the stiffness of carbon fiber.
    - I wrongly excluded 3D printing functional parts because I was using BambuLab's PLA Aero which even though described as "Tough with Good Impact Resistance" is very weak and brittle. Printing with PETG made me realise that 3D printed parts can be strong enough to take some load.
    - I was not really happy with FreeCad, I found it slow and unintuitive.
- Electronics design
    - The electronics design was functional, but way too bulky
    - I tried to have all the connectors accessible from the outside of the fuselage for ease of disassembly, this was a bad idea for aerodynamics, weight, and electronics layout.
- Software
    - I planned to use ROS, but compilation times were waaaay too long. I knew they could be because i worked on a big ROS project before but i expected them to be manageable for a small project. They are not. Cross-compiling a simple project for arm64 can take several minutes if you add custom message declarations.  
- Overall
    - I realised how hazardous flying such a big plane could be. My design lacked fail-safes and redundancies
    - I realised using an Xbox controller to fly the plane was not a very good idea as it lacked latched inputs for throttle, and control modes.