#pragma once

#include <rpi_pico_utils/clock/ClockInterface.h>
#include <rpi_pico_utils/io/IoInterface.h>
#include <rpi_pico_utils/pwm/PwmInterface.h>
#include <rpi_pico_utils/gpio/InputGpioInterface.h>
#include <rpi_pico_utils/gpio/OutputGpioInterface.h>

struct HardwareInterface
{
    ClockInterface* clock;
    
    IoInterface* rf_uart;
    IoInterface* pc_uart;

    PwmInterface* esc_pwm;
    PwmInterface* left_flaperon_pwm;
    PwmInterface* right_flaperon_pwm;
    PwmInterface* elevator_pwm;
    PwmInterface* rudder_pwm;

    InputGpioInterface* primary_imu_int_pin;
    OutputGpioInterface* primary_imu_rst_pin;
    IoInterface* primary_imu_i2c;

    InputGpioInterface* backup_imu_int_pin;
    OutputGpioInterface* backup_imu_rst_pin;
    IoInterface* backup_imu_i2c;

    IoInterface* ads1115_i2c;

    OutputGpioInterface* system_led;
};