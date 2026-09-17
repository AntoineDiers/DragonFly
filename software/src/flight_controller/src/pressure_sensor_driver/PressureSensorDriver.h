#pragma once

#include <rpi_pico_utils/io/IoInterface.h>
#include <rpi_pico_utils/utils/Periodic.h>
#include <rpi_pico_utils/timer/Timer.h>
#include <rpi_pico_utils/gpio/InputGpioInterface.h>

#include <iostream>

class PressureSensorDriver
{
public:

    static constexpr uint32_t READ_PERIOD_MS = 80;

    PressureSensorDriver(
        ClockInterface* clock,
        IoInterface* i2c,
        InputGpioInterface* int_pin,
        Periodic<float>& pressure_Pa) :
        _clock(clock),
        _i2c(i2c),
        _int_pin(int_pin),
        _pressure_Pa(pressure_Pa),
        _config_timer(clock, 1000) 
    {
        configure();
        getCompensationData();
    }

    void tick()
    {
        if(_config_timer.poll())
        {
            configure();
            if(!_compensation_data) { getCompensationData(); }
        }

        if(!_compensation_data) { return; }
        if(!_int_pin->read()) { return; }
        
        uint8_t int_status;
        if(!read(0x11, &int_status, 1) || !(int_status & 0b1000)) { return; }

        uint8_t raw_data[6];
        if(!read(0x04, raw_data, sizeof(raw_data))) { return; }

        uint32_t raw_pressure =
            (uint32_t)raw_data[0] |
            ((uint32_t)raw_data[1] << 8) |
            ((uint32_t)raw_data[2] << 16);

        uint32_t raw_temperature =
            (uint32_t)raw_data[3] |
            ((uint32_t)raw_data[4] << 8) |
            ((uint32_t)raw_data[5] << 16);

        float pressure_Pa = compensatePressure(raw_pressure, raw_temperature, _compensation_data.value());
        _pressure_Pa.update(pressure_Pa);
    }

private:

    struct CalibrationData
    {
        uint16_t T1;
        uint16_t T2;
        int8_t   T3;


        int16_t P1;
        int16_t P2;
        int8_t P3;
        int8_t P4;
        uint16_t P5;
        uint16_t P6;
        int8_t P7;
        int8_t P8;
        int16_t P9;
        int8_t P10;
        int8_t P11;
    };

    struct CompensationData
    {
        float par_t1;
        float par_t2;
        float par_t3;

        float par_p1;
        float par_p2;
        float par_p3;
        float par_p4;
        float par_p5;
        float par_p6;
        float par_p7;
        float par_p8;
        float par_p9;
        float par_p10;
        float par_p11;

        float t_lin = 0.0f;
    };

    void configure()
    {
        _config_ok = true;

        // INT_CTRL : 
        //      bit 0 :     Push-Pull
        //      bit 1 :     Active high
        //      bit 2 :     Latching on
        //      bit 3 :     Fifo watermark INT disabled
        //      bit 4 :     Fifo Full INT disabled
        //      bit 5 :     int_ds = 0
        //      bit 6 :     Data Ready INT enabled
        //
        //                     reg   INT_CTRL
        uint8_t INT_CTRL[2] = {0x19, 0b01000110};
        _config_ok = _config_ok && write(INT_CTRL, sizeof(INT_CTRL));

        // PWR_CTRL : 
        //      bit 0 :     Pressure enabled
        //      bit 1 :     Temperature enabled
        //      bit 4..5 :  Normal mode
        //
        //                            reg   PWR_CTRL
        uint8_t PWR_CTRL[] = {0x1B, 0b00110011};
        _config_ok = _config_ok &&  write(PWR_CTRL, sizeof(PWR_CTRL));
        
        // OSR : 
        //      bit 0..2 :  Pressure oversampling = x32
        //      bit 5..3 :  Temperature oversampling = x1
        // 
        //                            reg   OSR
        uint8_t OSR[] = {0x1C, 0b00000101};
        _config_ok = _config_ok &&  write(OSR, sizeof(OSR));

        // ODR : 
        //      0x04 :      Data rate = 80 ms
        //
        //                            reg   ODR
        uint8_t ODR[] = {0x1D, 0x04};
        _config_ok = _config_ok &&  write(ODR, sizeof(ODR));

        // CONFIG : 
        //      bit 1..3 :  IIR Filter coeff = 1
        //
        //                     reg   INT_CTRL
        uint8_t CONFIG[2] = {0x1F, 0b00000010};
        _config_ok = _config_ok &&  write(CONFIG, sizeof(CONFIG));
    }

    void getCompensationData()
    {
        CalibrationData calibration_data;
        uint8_t raw_calibration_data[21];
        if(read(0x31, raw_calibration_data, sizeof(raw_calibration_data)))
        {
            calibration_data.T1 = (uint16_t)raw_calibration_data[0] | ((uint16_t)raw_calibration_data[1] << 8);
            calibration_data.T2 = (uint16_t)raw_calibration_data[2] | ((uint16_t)raw_calibration_data[3] << 8);
            calibration_data.T3 = (int8_t)raw_calibration_data[4];

            calibration_data.P1 =  (int16_t)((uint16_t)raw_calibration_data[5] + ((uint16_t)raw_calibration_data[6] << 8));
            calibration_data.P2 =  (int16_t)((uint16_t)raw_calibration_data[7] + ((uint16_t)raw_calibration_data[8] << 8));
            calibration_data.P3 =  (int8_t)raw_calibration_data[9];
            calibration_data.P4 =  (int8_t)raw_calibration_data[10];
            calibration_data.P5 =  (uint16_t)((uint16_t)raw_calibration_data[11] + ((uint16_t)raw_calibration_data[12] << 8));
            calibration_data.P6 =  (uint16_t)((uint16_t)raw_calibration_data[13] + ((uint16_t)raw_calibration_data[14] << 8));
            calibration_data.P7 =  (int8_t)raw_calibration_data[15];
            calibration_data.P8 =  (int8_t)raw_calibration_data[16];
            calibration_data.P9 =  (int16_t)((uint16_t)raw_calibration_data[17] + ((uint16_t)raw_calibration_data[18] << 8));
            calibration_data.P10 = (int8_t)raw_calibration_data[19];
            calibration_data.P11 = (int8_t)raw_calibration_data[20];

            _compensation_data = calculateCompensationData(calibration_data);
        }
    }

    CompensationData calculateCompensationData(const CalibrationData& calibration_data)
    {
        CompensationData res;

        res.par_t1 = (float)calibration_data.T1 * 256.0f;
        res.par_t2 = (float)calibration_data.T2 / 1073741824.0f; // 2^30
        res.par_t3 = (float)calibration_data.T3 / 281474976710656.0f; // 2^48

        res.par_p1 = ((float)calibration_data.P1 - 16384.0f) / 1048576.0f; // 2^20
        res.par_p2 = ((float)calibration_data.P2 - 16384.0f) / 536870912.0f; // 2^29
        res.par_p3 = (float)calibration_data.P3 / 4294967296.0f; // 2^32
        res.par_p4 = (float)calibration_data.P4 / 137438953472.0f; // 2^37
        res.par_p5 = (float)calibration_data.P5 * 8.0f; // 2^-3
        res.par_p6 = (float)calibration_data.P6 / 64.0f; // 2^6
        res.par_p7 = (float)calibration_data.P7 / 256.0f; // 2^8
        res.par_p8 = (float)calibration_data.P8 / 32768.0f; // 2^15
        res.par_p9 = (float)calibration_data.P9 / 281474976710656.0f; // 2^48
        res.par_p10 = (float)calibration_data.P10 / 281474976710656.0f; // 2^48
        res.par_p11 = (float)calibration_data.P11 / 36893488147419103232.0f; // 2^65

        return res;
    }

    static float compensatePressure(
        uint32_t raw_pressure,
        uint32_t raw_temperature,
        const CompensationData& compensation_data)
    {
        // Temperature compensation

        float temperature_difference =
            (float)raw_temperature - compensation_data.par_t1;

        float temperature_compensation =
            temperature_difference * compensation_data.par_t2;

        float t_lin =
            temperature_compensation +
            temperature_difference *
            temperature_difference *
            compensation_data.par_t3;


        // Pressure compensation

        float pressure_temperature_term_1 =
            compensation_data.par_p6 * t_lin;

        float pressure_temperature_term_2 =
            compensation_data.par_p7 * t_lin * t_lin;

        float pressure_temperature_term_3 =
            compensation_data.par_p8 * t_lin * t_lin * t_lin;

        float pressure_temperature_compensation =
            compensation_data.par_p5 +
            pressure_temperature_term_1 +
            pressure_temperature_term_2 +
            pressure_temperature_term_3;


        float pressure_temperature_term_4 =
            compensation_data.par_p2 * t_lin;

        float pressure_temperature_term_5 =
            compensation_data.par_p3 * t_lin * t_lin;

        float pressure_temperature_term_6 =
            compensation_data.par_p4 * t_lin * t_lin * t_lin;

        float pressure_scale =
            compensation_data.par_p1 +
            pressure_temperature_term_4 +
            pressure_temperature_term_5 +
            pressure_temperature_term_6;

        float pressure_linear_term =
            (float)raw_pressure * pressure_scale;


        float raw_pressure_squared =
            (float)raw_pressure * (float)raw_pressure;

        float pressure_nonlinear_coefficient =
            compensation_data.par_p9 +
            compensation_data.par_p10 * t_lin;

        float pressure_squared_term =
            raw_pressure_squared * pressure_nonlinear_coefficient;

        float pressure_cubed =
            raw_pressure_squared * (float)raw_pressure;

        float pressure_cubed_term =
            pressure_cubed * compensation_data.par_p11;


        return pressure_temperature_compensation +
            pressure_linear_term +
            pressure_squared_term +
            pressure_cubed_term;
    }

    bool write(uint8_t* buffer, uint32_t n_bytes)
    {
        uint32_t n;
        return _i2c->write(buffer, n_bytes, n) && n == n_bytes;
    }

    bool read(uint8_t reg, uint8_t* buffer, uint32_t n_bytes)
    {
        uint32_t n;
        if(!_i2c->write(&reg, 1, n) || n != 1) { return false; }
        if(!_i2c->read(buffer, n_bytes, n) || n!=n_bytes) { return false; }
        return true;
    }

    ClockInterface* _clock;
    IoInterface* _i2c;
    InputGpioInterface* _int_pin;
    Periodic<float>& _pressure_Pa;

    bool _config_ok = false;
    std::optional<CompensationData> _compensation_data;

    Timer _config_timer;
};