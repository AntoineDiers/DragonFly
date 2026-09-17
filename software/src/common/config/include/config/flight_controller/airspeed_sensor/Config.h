#pragma once

#include <cstdint>

#include <rpi_pico_utils/i2c/I2cImplementation.h>

#include "../../../flight_controller/src/airspeed_sensor_driver/AirspeedSensorDriver.h"

namespace config::flight_controller::airspeed_sensor
{
    static constexpr uint8_t I2C_SLAVE_ADDR = 0x28;
    static constexpr AirspeedSensorDriver::Config CONFIG
    {
        .read_period_ms = 100
    };
}