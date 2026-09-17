#include <gtest/gtest.h>

#include "ImuMock.h"
#include <rpi_pico_utils/clock/ClockMock.h>

TEST(FlightController_ImuDriver, CanReadImuData)
{
    ClockMock clock;

    FlightControllerInputs::Attitude attitude
    {
        .roll_deg = 10,
        .pitch_deg = -15
    };
    ImuMock imu_mock(&clock);
    imu_mock.setAttitude(attitude);

    PeriodicDataConfig output_rate_config
    {
        .expected_rate_hz = 100,
        .max_rate_deviation_hz = 10,
        .rate_sliding_window_s = 1,
    };
    Periodic<FlightControllerInputs::Attitude> output(output_rate_config, &clock);

    ImuDriver::Config config
    {
        .reset_signal_duration_ms = 10,
        .reset_duration_ms = 1000,
        .game_rotation_vector_timeout_ms = 100,
        .read_period_ms = 10,
    };
    ImuDriver imu_driver(config, &output, &clock, &imu_mock, imu_mock.intPin(), imu_mock.rstPin());

    while(clock.getTime_us() < 10000000)
    {
        imu_mock.tick();
        imu_driver.tick();
        clock.advance_ms(1);
    }

    ASSERT_TRUE(output.getState().is_ok);
    ASSERT_EQ(imu_mock.resetCount(), 0);
    ASSERT_NEAR(output.getLastValue()->roll_deg, attitude.roll_deg, 1);
    ASSERT_NEAR(output.getLastValue()->pitch_deg, attitude.pitch_deg, 1);
    
}

TEST(FlightController_ImuDriver, CanResetOnTimeout)
{
    ClockMock clock;

    FlightControllerInputs::Attitude attitude
    {
        .roll_deg = 5,
        .pitch_deg = 20
    };
    ImuMock imu_mock(&clock);
    imu_mock.setAttitude(attitude);

    PeriodicDataConfig output_rate_config
    {
        .expected_rate_hz = 100,
        .max_rate_deviation_hz = 10,
        .rate_sliding_window_s = 1,
    };
    Periodic<FlightControllerInputs::Attitude> output(output_rate_config, &clock);

    ImuDriver::Config config
    {
        .reset_signal_duration_ms = 10,
        .reset_duration_ms = 1000,
        .game_rotation_vector_timeout_ms = 100,
        .read_period_ms = 10,
    };
    ImuDriver imu_driver(config, &output, &clock, &imu_mock, imu_mock.intPin(), imu_mock.rstPin());

    while(clock.getTime_us() < 10000000)
    {
        imu_mock.tick();
        imu_driver.tick();
        clock.advance_ms(1);
    }

    ASSERT_TRUE(output.getState().is_ok);
    ASSERT_EQ(imu_mock.resetCount(), 0);
    ASSERT_NEAR(output.getLastValue()->roll_deg, attitude.roll_deg, 1);
    ASSERT_NEAR(output.getLastValue()->pitch_deg, attitude.pitch_deg, 1);

    imu_mock.setAttitude(std::nullopt);

    while(clock.getTime_us() < 11000000)
    {
        imu_mock.tick();
        imu_driver.tick();
        clock.advance_ms(1);
    }

    ASSERT_FALSE(output.getState().is_ok);
    ASSERT_EQ(imu_mock.resetCount(), 1);

    imu_mock.setAttitude(attitude);

    while(clock.getTime_us() < 20000000)
    {
        imu_mock.tick();
        imu_driver.tick();
        clock.advance_ms(1);
    }

    ASSERT_TRUE(output.getState().is_ok);
    ASSERT_EQ(imu_mock.resetCount(), 1);
    ASSERT_NEAR(output.getLastValue()->roll_deg, attitude.roll_deg, 1);
    ASSERT_NEAR(output.getLastValue()->pitch_deg, attitude.pitch_deg, 1);
    
}