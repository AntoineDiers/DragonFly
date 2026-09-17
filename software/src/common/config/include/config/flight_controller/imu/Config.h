#pragma once

#include <cstdint>

#include <rpi_pico_utils/i2c/I2cImplementation.h>

#include "../gpio/Config.h"
#include "../../../flight_controller/src/imu_driver/ImuDriver.h"

namespace config::flight_controller::imu
{
    static constexpr uint8_t I2C_SLAVE_ADDR = 0x4A;
    static constexpr ImuDriver::Config IMU_DRIVER 
    {
        .reset_signal_duration_ms = 10,
        .reset_duration_ms = 1000,
        .game_rotation_vector_timeout_ms = 500,
        .read_period_ms = 10,
    };
}