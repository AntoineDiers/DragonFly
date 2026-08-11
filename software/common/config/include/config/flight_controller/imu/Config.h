#pragma once

#include <cstdint>

#include <rpi_pico_utils/i2c/I2cImplementation.h>

#include "../gpio/Config.h"

namespace config::flight_controller::imu
{
    I2cImplementation::Config I2C
    {
        .i2c_inst = i2c0,
        .scl_gpio = gpio::IMU_SCL_GPIO,
        .sda_gpio = gpio::IMU_SDA_GPIO,
        .addr = 0x4A,
        .baudrate = 400000,
        .rw_timeout_ms = 10,
    };

    static constexpr uint32_t GAME_ROTATION_VECTOR_TIMEOUT_MS = 500;
    static constexpr uint32_t RESET_SIGNAL_DURATION_MS = 10;
    static constexpr uint32_t RESET_DURATION_MS = 1000;
}