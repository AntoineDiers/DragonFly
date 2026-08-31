#include <gtest/gtest.h>

#include "Ads1115Mock.h"

#include <rpi_pico_utils/clock/ClockMock.h>
#include <rpi_pico_utils/utils/Periodic.h>

TEST(Common_Ads1115Driver, CanReadInputs)
{
    Ads1115Driver::Config config
    {
        .a0_config = Ads1115Driver::PinConfig 
        {
            .read_period_ms = 100,
            .sps = Ads1115Driver::SPS::SPS_64,
        },
        .a1_config = std::nullopt,
        .a2_config = Ads1115Driver::PinConfig 
        {
            .read_period_ms = 10,
            .sps = Ads1115Driver::SPS::SPS_128,
        },
        .a3_config = std::nullopt
    };

    ClockMock clock;
    Ads1115Mock mock(&clock);
    Ads1115Driver driver(&clock, &mock, config);

    PeriodicDataConfig expected_period 
    { 
        .expected_rate_hz = 100,
        .max_rate_deviation_hz = 5,
        .rate_sliding_window_s = 1
    };
    Periodic<float> a2_output(expected_period, &clock);

    while(clock.getTime_us() < 1000000)
    {
        float voltage = 3 * sin(clock.getTime_us() / 1000000.0);
        mock.setPinVoltage(Ads1115Driver::Pin::A2, voltage);

        std::optional<Ads1115Driver::Reading> reading = driver.tick();
        if(reading)
        {
            ASSERT_EQ(reading->pin, Ads1115Driver::Pin::A2);
            ASSERT_NEAR(reading->voltage, voltage, 0.01);
            a2_output.update(reading->voltage);
        }

        clock.advance_ms(1);
    }

    ASSERT_TRUE(a2_output.getState().is_ok);
}