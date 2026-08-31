#pragma once

#include <rpi_pico_utils/clock/ClockInterface.h>
#include <rpi_pico_utils/io/IoInterface.h>
#include <rpi_pico_utils/gpio/InputGpioInterface.h>
#include <rpi_pico_utils/gpio/OutputGpioInterface.h>
#include <rpi_pico_utils/pwm/PwmInterface.h>
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
    
    PwmInterface* link_led_r;
    PwmInterface* link_led_g;
    PwmInterface* status_led_r;
    PwmInterface* status_led_g;
    OutputGpioInterface* pico_led;
};