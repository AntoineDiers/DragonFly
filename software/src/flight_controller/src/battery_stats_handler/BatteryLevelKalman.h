#pragma once

#include <rpi_pico_utils/utils/Matrix.h>
#include <rpi_pico_utils/clock/ClockInterface.h>

#include <config/flight_controller/battery/Config.h>

#include <algorithm>
#include <optional>

#include "../inputs/FlightControllerInputs.h"

class BatteryLevelKalman
{
public:

    struct Config
    {
        const LookUpTableInterface* charge_voltage_lut;

        float capacity_mAh;
        float min_innovation_covariance;
        float timeout_s;

        float voltage_std_dev;
        float current_std_dev;
        float voc_std_dev;
        float vpol_std_dev;
        float voc_std_dev_init;
        float vpol_std_dev_init;

        float r_int;
        float r_pol;
        float c_pol;
    };

    struct Input
    {
        float current;
        float voltage;
    };

    BatteryLevelKalman(const Config& config, ClockInterface* clock) : 
        _config(config),
        _clock(clock)
    {
        reset();
    }

    FlightControllerInputs::BatteryLevel tick(const Input& input)
    {
        std::optional<uint64_t> dt_us;
        if(_last_tick_stamp.has_value())
        {
            dt_us = _clock->getTime_us() - _last_tick_stamp.value();
        }
        _last_tick_stamp = _clock->getTime_us();

        if(dt_us && dt_us.value() > _config.timeout_s * 1000000)
        {
            reset();
        }
        else if(dt_us)
        {
            float dt_s = dt_us.value() / 1000000.0;

            float estimated_Voc = _kalman_state.at<0,0>();
            float estimated_level = _config.charge_voltage_lut->getX(estimated_Voc);
            float dQ = 3600 * (_config.capacity_mAh / 1000) * 1.0 / 100.0;
            float dVoc = estimated_Voc - _config.charge_voltage_lut->getY(estimated_level - 1.0);
            float dVoc_dQ = dVoc / dQ;

            // Prediction
            Matrix<1,1> u = Matrix<1,1>::zero();
            u.at<0,0>() = input.current;

            Matrix<2,2> F = Matrix<2,2>::zero();
            F.at<0,0>() = 1.0f;
            F.at<1,1>() = 1.0f - dt_s / (_config.r_pol * _config.c_pol);

            Matrix<2,1> B = Matrix<2,1>::zero();
            B.at<0,0>() = - dt_s * dVoc_dQ;
            B.at<1,0>() = dt_s / _config.c_pol;

            Matrix<2,2> Q = Matrix<2,2>::zero();
            Q.at<0,0>() = dt_s * dt_s * dVoc_dQ * dVoc_dQ * _config.current_std_dev * _config.current_std_dev + _config.voc_std_dev * _config.voc_std_dev;
            Q.at<0,1>() = - dt_s * dt_s * _config.current_std_dev * _config.current_std_dev * dVoc_dQ / _config.c_pol;
            Q.at<1,0>() = - dt_s * dt_s * _config.current_std_dev * _config.current_std_dev * dVoc_dQ / _config.c_pol;
            Q.at<1,1>() = dt_s * dt_s * _config.current_std_dev * _config.current_std_dev / (_config.c_pol * _config.c_pol) + _config.vpol_std_dev * _config.vpol_std_dev;

            Matrix<2,1> predicted_state = F.mult(_kalman_state) + B.mult(u);
            Matrix<2,2> predicted_covariance = F.mult(_kalman_covariance).mult(F.transpose()) + Q;

            // Update

            Matrix<1,1> z = Matrix<1,1>::zero();
            z.at<0,0>() = input.voltage + _config.r_int * input.current;
            
            Matrix<1,2> H = Matrix<1,2>::zero();
            H.at<0,0>() = 1.0f;
            H.at<0,1>() = -1.0f;

            Matrix<1,1> R = Matrix<1,1>::zero();
            R.at<0,0>() = _config.voltage_std_dev * _config.voltage_std_dev + _config.r_int * _config.r_int * _config.current_std_dev * _config.current_std_dev;

            Matrix<1,1> innovation = z - H.mult(predicted_state);
            Matrix<1,1> innovation_covariance = H.mult(predicted_covariance).mult(H.transpose()) + R;
            innovation_covariance.at<0,0>() = std::max(innovation_covariance.at<0,0>(), _config.min_innovation_covariance);

            Matrix<2,1> kalman_gain = predicted_covariance.mult(H.transpose()) / innovation_covariance.at<0,0>();

            _kalman_state = predicted_state + kalman_gain.mult(innovation);
            _kalman_covariance = (Matrix<2,2>::identity() - kalman_gain.mult(H)).mult(predicted_covariance);
        }

        FlightControllerInputs::BatteryLevel res;
        res.level_percent = _config.charge_voltage_lut->getX(_kalman_state.at<0,0>());
        res.level_std_dev = res.level_percent - _config.charge_voltage_lut->getX(_kalman_state.at<0,0>() - std::sqrt(_kalman_covariance.at<0,0>()));

        return res;
    }

private: 

    void reset()
    {
        _kalman_state.at<0,0>() = _config.charge_voltage_lut->getY(100);

        _kalman_covariance.at<0,0>() = _config.voc_std_dev_init * _config.voc_std_dev_init;
        _kalman_covariance.at<1,1>() = _config.vpol_std_dev_init * _config.vpol_std_dev_init;
    }

    Config _config;
    ClockInterface* _clock;

    std::optional<uint64_t> _last_tick_stamp;

    Matrix<2,1> _kalman_state = Matrix<2,1>::zero();
    Matrix<2,2> _kalman_covariance = Matrix<2,2>::zero();
};