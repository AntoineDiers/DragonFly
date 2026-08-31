#include "RemoteControl.h"

#include <rpi_pico_utils/clock/ClockImplementation.h>
#include <rpi_pico_utils/watchdog/WatchdogImplementation.h>
#include <rpi_pico_utils/uart/UartImplementation.h>
#include <rpi_pico_utils/usb/UsbImplementation.h>
#include <rpi_pico_utils/i2c/I2cImplementation.h>
#include <rpi_pico_utils/adc/AdcImplementation.h>
#include <rpi_pico_utils/gpio/InputGpioImplementation.h>
#include <rpi_pico_utils/pwm/PwmImplementation.h>
#include <rpi_pico_utils/gpio/OutputGpioImplementation.h>

#include <config/remote_control/leds/Config.h>
#include <config/remote_control/comms/Config.h>
#include <config/common/watchdog/Config.h>

#include <pico/stdio.h>
#include <hardware/watchdog.h>
#include <string>

void sendCommand(UartImplementation& uart, const std::string& cmd)
{
    uint32_t n_bytes_written;
    uart.write((uint8_t*)cmd.data(), cmd.size(), n_bytes_written);

    std::cout << " -> " << cmd << "\n";
    std::cout << " <- " "\n";

    std::optional<uint64_t> last_response;
    while(!last_response || time_us_64() < last_response.value() + 1000000)
    {
        uint32_t n_bytes_read;
        uint8_t buffer[100];
        if(uart.read(buffer, sizeof(buffer), n_bytes_read) && n_bytes_read > 0)
        {
            last_response = time_us_64();
            std::cout << std::string(buffer, buffer + n_bytes_read);
        }
    }
}

int main()
{
    stdio_init_all();

    ClockImplementation clock;
    WatchdogImplementation watchdog;
    UartImplementation rf_uart(config::remote_control::comms::RF_UART_CONFIG);
    UsbImplementation gs_usb;
    I2cImplementation i2c_0(config::remote_control::comms::I2C_CONFIG);
    I2cImplementation::SlaveHandle ads1115_i2c = i2c_0.generateSlaveHandle(config::remote_control::inputs::ads_1115::SLAVE_ADDR);
    AdcImplementation<config::remote_control::gpio::THROTTLE_ADC> throttle_adc;
    AdcImplementation<config::remote_control::gpio::LEFT_RIGHT_ADC> left_right_adc;
    AdcImplementation<config::remote_control::gpio::DOWN_UP_ADC> down_up_adc;
    InputGpioImplementation arm_switch(config::remote_control::gpio::ARM_SWITCH);
    InputGpioImplementation auto_switch(config::remote_control::gpio::AUTO_SWITCH);
    PwmImplementation link_led_r(config::remote_control::leds::LINK_RED_PWM_CONFIG);
    PwmImplementation link_led_g(config::remote_control::leds::LINK_GREEN_PWM_CONFIG);
    PwmImplementation status_led_r(config::remote_control::leds::STATUS_RED_PWM_CONFIG);
    PwmImplementation status_led_g(config::remote_control::leds::STATUS_GREEN_PWM_CONFIG);
    OutputGpioImplementation pico_led(config::remote_control::gpio::PICO_LED);

    HardwareInterface hw_interface
    {
        .clock = &clock,
        .watchdog = &watchdog,
        .rf_uart = &rf_uart,
        .gs_usb = &gs_usb,
        .ads_1115_i2c = &ads1115_i2c,
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