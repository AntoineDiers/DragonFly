#pragma once

#include <rpi_pico_utils/clock/ClockInterface.h>
#include <rpi_pico_utils/io/IoInterface.h>
#include <rpi_pico_utils/gpio/InputGpioInterface.h>
#include <rpi_pico_utils/gpio/OutputGpioInterface.h>
#include <rpi_pico_utils/adc/AdcInterface.h>
#include <rpi_pico_utils/watchdog/WatchdogInterface.h>

struct HardwareInterface
{
    ClockInterface* clock;
    WatchdogInterface* watchdog;
    
    IoInterface* rf_uart;
    IoInterface* gs_usb;

    IoInterface* ads_1115_i2c;

    AdcInterface* throttle_stick;
    AdcInterface* left_right_stick;
    AdcInterface* down_up_stick;

    InputGpioInterface* arm_switch;
    InputGpioInterface* auto_switch;
    
    OutputGpioInterface* link_led_r;
    OutputGpioInterface* link_led_g;
    OutputGpioInterface* status_led_r;
    OutputGpioInterface* status_led_g;
    OutputGpioInterface* pico_led;
};