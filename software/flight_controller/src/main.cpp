#include "FlightController.h"

#include <rpi_pico_utils/clock/ClockImplementation.h>
#include <rpi_pico_utils/uart/UartImplementation.h>
#include <rpi_pico_utils/pwm/PwmImplementation.h>
#include <rpi_pico_utils/gpio/InputGpioImplementation.h>
#include <rpi_pico_utils/gpio/OutputGpioImplementation.h>
#include <rpi_pico_utils/i2c/I2cImplementation.h>

#include <pico/stdlib.h>

int main()
{
    stdio_init_all();

    // CLOCK
    ClockImplementation clock;

    // UART DRIVERS
    UartImplementation rf_uart(config::flight_controller::comms::RF_UART_CONFIG);
    UartImplementation pc_uart(config::flight_controller::comms::PAYLOAD_COMPUTER_UART_CONFIG);

    // ACTUATORS PWMS
    PwmImplementation esc_pwm(config::flight_controller::actuators::ESC_PWM);
    PwmImplementation left_flaperon_pwm(config::flight_controller::actuators::LEFT_FLAPERON_PWM);
    PwmImplementation right_flaperon_pwm(config::flight_controller::actuators::RIGHT_FLAPERON_PWM);
    PwmImplementation elevator_pwm(config::flight_controller::actuators::ELEVATOR_PWM);
    PwmImplementation rudder_pwm(config::flight_controller::actuators::RUDDER_PWM);

    // IMU
    InputGpioImplementation imu_int_pin(config::flight_controller::gpio::IMU_INT_GPIO);
    OutputGpioImplementation imu_rst_pin(config::flight_controller::gpio::IMU_RST_GPIO);
    I2cImplementation imu_i2c(config::flight_controller::imu::I2C);

    HardwareInterface hardware_interface = 
    {
        .clock =                &clock,

        .rf_uart =              &rf_uart,
        .pc_uart =              &pc_uart,

        .esc_pwm =              &esc_pwm,
        .left_flaperon_pwm =    &left_flaperon_pwm,
        .right_flaperon_pwm =   &right_flaperon_pwm,
        .elevator_pwm =         &elevator_pwm,
        .rudder_pwm =           &rudder_pwm,

        .imu_int_pin =          &imu_int_pin,
        .imu_rst_pin =          &imu_rst_pin,
        .imu_i2c =              &imu_i2c,
    };
    
    FlightController flight_controller(hardware_interface);

    while(true)
    {
        flight_controller.tick();
    }
}