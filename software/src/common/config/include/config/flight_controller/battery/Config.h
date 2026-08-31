#pragma once

#include <rpi_pico_utils/utils/LookUpTable.h>
#include "../../../flight_controller/src/battery_stats_handler/BatteryStatsHandler.h"
#include <rpi_pico_utils/utils/Periodic.h>

#include "../gpio/Config.h"

namespace config::flight_controller::battery
{   
    static constexpr uint8_t ADS1115_SLAVE_ADDR = 0x05;

    static constexpr Ads1115Driver::PinConfig ADS_1115_PINS_CONFIG
    {
        .read_period_ms = 100,
        .sps = Ads1115Driver::SPS::SPS_64
    };

    static const LookUpTable<19> CHARGE_VOLTAGE_LUT (
    {
        LookUpTableInterface::Point{10, 14.76},
        LookUpTableInterface::Point{15, 14.84},
        LookUpTableInterface::Point{20, 14.92},
        LookUpTableInterface::Point{25, 15.00},
        LookUpTableInterface::Point{30, 15.08},
        LookUpTableInterface::Point{35, 15.16},
        LookUpTableInterface::Point{40, 15.20},
        LookUpTableInterface::Point{45, 15.28},
        LookUpTableInterface::Point{50, 15.36},
        LookUpTableInterface::Point{55, 15.40},
        LookUpTableInterface::Point{60, 15.48},
        LookUpTableInterface::Point{65, 15.64},
        LookUpTableInterface::Point{70, 15.80},
        LookUpTableInterface::Point{75, 15.92},
        LookUpTableInterface::Point{80, 16.08},
        LookUpTableInterface::Point{85, 16.32},
        LookUpTableInterface::Point{90, 16.44},
        LookUpTableInterface::Point{95, 16.60},
        LookUpTableInterface::Point{100, 16.80},
    });

    static constexpr BatteryStatsHandler::Config STATS_HANDLER_CONFIG
    {
        .ads1115_config = 
        {
            .a0_config = ADS_1115_PINS_CONFIG,
            .a1_config = ADS_1115_PINS_CONFIG,
            .a2_config = ADS_1115_PINS_CONFIG,
            .a3_config = ADS_1115_PINS_CONFIG
        },
        .voltage_pin = Ads1115Driver::Pin::A0,
        .current_pin = Ads1115Driver::Pin::A0,
        .current_after_esc_pin = Ads1115Driver::Pin::A0,
        .current_after_servos_pin = Ads1115Driver::Pin::A0,
        .battery_voltage_divider_ratio = 10.0f / (33.0f + 10.0f),
        .acs770_zero_v = 0.5,
        .acs770_sensitivity_a_per_v = 0.04,
        .acs712_zero_v = 2.5,
        .acs712_sensitivity_a_per_v = 0.066,
        .ads1115_data_timeout_ms = 300,
        .kalman_period_ms = 100,
        .kalman_config = 
        {
            .charge_voltage_lut = &CHARGE_VOLTAGE_LUT,

            .capacity_mAh = 20000,
            .min_innovation_covariance = 0.0001,
            .timeout_s = 3,

            .voltage_std_dev = 0.01,
            .current_std_dev = 0.1,
            .voc_std_dev = 0.01,
            .vpol_std_dev = 0.01,
            .voc_std_dev_init = 1,
            .vpol_std_dev_init = 1,

            .r_int = 0.015,
            .r_pol = 0.01,
            .c_pol = 1000,
        }
    };
}