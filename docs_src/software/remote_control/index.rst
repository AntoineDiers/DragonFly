Remote Control
-----------------------------

The remote control must perform the following functions :

- Read its internal sensors periodically, and write the values in an "inputs" message
- Send this message to the Flight Controller via the RF or the 4G link depending on links quality
- Forward messages from the Ground Computer to the DragonFly computers (FC and PC)
- Update its status LED depending on the state of its internal sensors
- Update its link LED depending on the link quality with the Flight Controller

