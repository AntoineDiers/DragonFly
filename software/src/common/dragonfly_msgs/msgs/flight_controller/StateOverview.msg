#include <dragonfly_msgs/enums/ControlModes.h>
#include <dragonfly_msgs/msgs/flight_controller/Actuators.h>
#include <dragonfly_msgs/msgs/flight_controller/Navigation.h>
#include <dragonfly_msgs/msgs/flight_controller/InputDiagsOverview.h>

base_types::Enum<enums::ControlModes>     control_mode
Actuators               actuators
Navigation              navigation
InputDiagsOverview      input_diags
base_types::Int<0,100>  battery_level_percent