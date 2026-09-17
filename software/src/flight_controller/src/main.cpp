#include "FlightController.h"

#include <rpi_pico_utils/clock/ClockImplementation.h>
#include <rpi_pico_utils/uart/UartImplementation.h>
#include <rpi_pico_utils/pwm/PwmImplementation.h>
#include <rpi_pico_utils/gpio/InputGpioImplementation.h>
#include <rpi_pico_utils/gpio/OutputGpioImplementation.h>
#include <rpi_pico_utils/i2c/I2cImplementation.h>
#include <rpi_pico_utils/spi/SpiImplementation.h>
#include <rpi_pico_utils/timer/Timer.h>

#include <config/flight_controller/gpio/Config.h>
#include <config/flight_controller/comms/Config.h>
#include <config/flight_controller/imu/Config.h>
#include <config/flight_controller/airspeed_sensor/Config.h>
#include <config/common/pressure_sensor/Config.h>

#include <pico/stdlib.h>

/*void sendCommand(UartImplementation& uart, const std::string& cmd)
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
}*/

int main()
{
    stdio_init_all();

    // CLOCK
    ClockImplementation clock;

    // UART DRIVERS
    UartImplementation rf_uart(config::flight_controller::comms::RF_UART_CONFIG);
    UartImplementation gps_uart(config::flight_controller::comms::GPS_UART_CONFIG);

    // PAYLOAD COMPUTER SPI
    SpiImplementation pc_spi(config::flight_controller::comms::PAYLOAD_COMPUTER_SPI_CONFIG);

    // ACTUATORS PWMS
    PwmImplementation esc_pwm(config::flight_controller::actuators::ESC_PWM);
    PwmImplementation left_flaperon_pwm(config::flight_controller::actuators::LEFT_FLAPERON_PWM);
    PwmImplementation right_flaperon_pwm(config::flight_controller::actuators::RIGHT_FLAPERON_PWM);
    PwmImplementation left_ruddervator_pwm(config::flight_controller::actuators::LEFT_RUDDERVATOR_PWM);
    PwmImplementation right_ruddervator_pwm(config::flight_controller::actuators::RIGHT_RUDDERVATOR_PWM);

    //I2C
    I2cImplementation i2c_0(config::flight_controller::comms::I2C_0);
    I2cImplementation i2c_1(config::flight_controller::comms::I2C_1);

    // PRIMARY IMU
    InputGpioImplementation primary_imu_int_pin(config::flight_controller::gpio::PRIMARY_IMU_INT);
    OutputGpioImplementation primary_imu_rst_pin(config::flight_controller::gpio::PRIMARY_IMU_RST);
    I2cImplementation::SlaveHandle primary_imu_i2c = i2c_1.generateSlaveHandle(config::flight_controller::imu::I2C_SLAVE_ADDR);

    // BACKUP IMU
    InputGpioImplementation backup_imu_int_pin(config::flight_controller::gpio::BACKUP_IMU_INT);
    OutputGpioImplementation backup_imu_rst_pin(config::flight_controller::gpio::BACKUP_IMU_RST);
    I2cImplementation::SlaveHandle backup_imu_i2c = i2c_0.generateSlaveHandle(config::flight_controller::imu::I2C_SLAVE_ADDR);

    // ADS1115
    I2cImplementation::SlaveHandle ads1115_i2c = i2c_0.generateSlaveHandle(config::flight_controller::battery::ADS1115_SLAVE_ADDR);

    // AIRSPEED SENSOR
    I2cImplementation::SlaveHandle airspeed_sensor_i2c = i2c_1.generateSlaveHandle(config::flight_controller::airspeed_sensor::I2C_SLAVE_ADDR);

    // PRESSURE SENSOR
    I2cImplementation::SlaveHandle pressure_sensor_i2c = i2c_1.generateSlaveHandle(config::common::pressure_sensor::I2C_SLAVE_ADDRESS);
    InputGpioImplementation pressure_sensor_int_pin(config::flight_controller::gpio::PRESSURE_SENSOR_INT);

    // SYSTEM LED
    OutputGpioImplementation system_led(config::flight_controller::gpio::SYSTEM_LED);

    HardwareInterface hardware_interface = 
    {
        .clock =                    &clock,

        .rf_uart =                  &rf_uart,
        .gps_uart =                 &gps_uart,

        .pc_spi =                   &pc_spi,

        .esc_pwm =                  &esc_pwm,
        .left_flaperon_pwm =        &left_flaperon_pwm,
        .right_flaperon_pwm =       &right_flaperon_pwm,
        .left_ruddervator_pwm =     &left_ruddervator_pwm,
        .right_ruddervator_pwm =    &right_ruddervator_pwm,

        .primary_imu_int_pin =      &primary_imu_int_pin,
        .primary_imu_rst_pin =      &primary_imu_rst_pin,
        .primary_imu_i2c =          &primary_imu_i2c,

        .backup_imu_int_pin =       &backup_imu_int_pin,
        .backup_imu_rst_pin =       &backup_imu_rst_pin,
        .backup_imu_i2c =           &backup_imu_i2c,

        .ads1115_i2c =              &ads1115_i2c, 
        .airspeed_sensor_i2c =      &airspeed_sensor_i2c,

        .pressure_sensor_i2c =      &pressure_sensor_i2c,
        .pressure_sensor_int_pin =  &pressure_sensor_int_pin,

        .system_led =               &system_led,
    };
    
    FlightController flight_controller(hardware_interface);

    while(true)
    {
        flight_controller.tick();
    }
}