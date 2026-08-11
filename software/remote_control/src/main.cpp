#include "RemoteControl.h"

#include <rpi_pico_utils/clock/ClockImplementation.h>
#include <rpi_pico_utils/watchdog/WatchdogImplementation.h>
#include <rpi_pico_utils/uart/UartImplementation.h>
#include <rpi_pico_utils/usb/UsbImplementation.h>
#include <rpi_pico_utils/i2c/I2cImplementation.h>
#include <rpi_pico_utils/adc/AdcImplementation.h>
#include <rpi_pico_utils/gpio/InputGpioImplementation.h>
#include <rpi_pico_utils/gpio/OutputGpioImplementation.h>

#include <config/remote_control/comms/Config.h>
#include <config/common/watchdog/Config.h>

#include <pico/stdio.h>
#include <hardware/watchdog.h>


int main()
{
    stdio_init_all();
    
    ClockImplementation clock;
    WatchdogImplementation watchdog;
    UartImplementation rf_uart(config::remote_control::comms::RF_UART_CONFIG);
    UsbImplementation gs_usb;
    I2cImplementation ads_1115_i2c(config::remote_control::inputs::ads_1115::I2C_CONFIG);
    AdcImplementation<config::remote_control::gpio::THROTTLE_ADC> throttle_adc;
    AdcImplementation<config::remote_control::gpio::LEFT_RIGHT_ADC> left_right_adc;
    AdcImplementation<config::remote_control::gpio::DOWN_UP_ADC> down_up_adc;
    InputGpioImplementation arm_switch(config::remote_control::gpio::ARM_SWITCH);
    InputGpioImplementation auto_switch(config::remote_control::gpio::AUTO_SWITCH);
    OutputGpioImplementation link_led_r(config::remote_control::gpio::LINK_LED_R);
    OutputGpioImplementation link_led_g(config::remote_control::gpio::LINK_LED_G);
    OutputGpioImplementation status_led_r(config::remote_control::gpio::STATUS_LED_R);
    OutputGpioImplementation status_led_g(config::remote_control::gpio::STATUS_LED_G);
    OutputGpioImplementation pico_led(config::remote_control::gpio::PICO_LED);

    HardwareInterface hw_interface
    {
        .clock = &clock,
        .watchdog = &watchdog,
        .rf_uart = &rf_uart,
        .gs_usb = &gs_usb,
        .ads_1115_i2c = &ads_1115_i2c,
        .throttle_stick = &throttle_adc,
        .left_right_stick = &left_right_adc,
        .down_up_stick = &down_up_adc,
        .arm_switch = &arm_switch,
        .auto_switch = &auto_switch,
        .link_led_r = &link_led_r,
        .link_led_g = &link_led_g,
        .status_led_r = &status_led_r,
        .status_led_g = &status_led_g,
        .pico_led = &pico_led
    };

    RemoteControl remote_control(hw_interface);

    while(true)
    {
        remote_control.tick();
    }

    return 0;
}