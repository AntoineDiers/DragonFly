#include "FlightController.h"

#include <rpi_pico_utils/clock/ClockImplementation.h>
#include <rpi_pico_utils/uart/UartImplementation.h>
#include <rpi_pico_utils/pwm/PwmImplementation.h>
#include <rpi_pico_utils/gpio/InputGpioImplementation.h>
#include <rpi_pico_utils/gpio/OutputGpioImplementation.h>
#include <rpi_pico_utils/i2c/I2cImplementation.h>

#include <pico/stdlib.h>

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

    //I2C
    I2cImplementation i2c_0(config::flight_controller::comms::I2C_0);
    I2cImplementation i2c_1(config::flight_controller::comms::I2C_1);

    // PRIMARY IMU
    InputGpioImplementation primary_imu_int_pin(config::flight_controller::gpio::PRIMARY_IMU_INT);
    OutputGpioImplementation primary_imu_rst_pin(config::flight_controller::gpio::PRIMARY_IMU_RST);
    I2cImplementation::SlaveHandle primary_imu_i2c = i2c_0.generateSlaveHandle(config::flight_controller::imu::I2C_SLAVE_ADDR);

    // BACKUP IMU
    InputGpioImplementation backup_imu_int_pin(config::flight_controller::gpio::BACKUP_IMU_INT);
    OutputGpioImplementation backup_imu_rst_pin(config::flight_controller::gpio::BACKUP_IMU_RST);
    I2cImplementation::SlaveHandle backup_imu_i2c = i2c_1.generateSlaveHandle(config::flight_controller::imu::I2C_SLAVE_ADDR);

    // ADS1115
    I2cImplementation::SlaveHandle ads1115_i2c = i2c_0.generateSlaveHandle(config::flight_controller::battery::ADS1115_SLAVE_ADDR);

    // SYSTEM LED
    OutputGpioImplementation system_led(config::flight_controller::gpio::SYSTEM_LED);

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

        .primary_imu_int_pin =  &primary_imu_int_pin,
        .primary_imu_rst_pin =  &primary_imu_rst_pin,
        .primary_imu_i2c =      &primary_imu_i2c,

        .backup_imu_int_pin =   &backup_imu_int_pin,
        .backup_imu_rst_pin =   &backup_imu_rst_pin,
        .backup_imu_i2c =       &backup_imu_i2c,

        .ads1115_i2c =          &ads1115_i2c,  

        .system_led =           &system_led,
    };
    
    FlightController flight_controller(hardware_interface);

    while(true)
    {
        flight_controller.tick();
    }
}