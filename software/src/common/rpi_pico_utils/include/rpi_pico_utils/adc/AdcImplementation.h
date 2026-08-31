#pragma once

#include "AdcInterface.h"
#include <cstdint>
#include <hardware/adc.h>

enum AdcPins
{
    ADC_0 = 26,
    ADC_1 = 27,
    ADC_2 = 28
};

template<uint8_t GpioPin>
class AdcImplementation : public AdcInterface
{
public:

    static_assert(
        GpioPin == AdcPins::ADC_0 ||
        GpioPin == AdcPins::ADC_1 ||
        GpioPin == AdcPins::ADC_2);

    AdcImplementation()
    {
        adc_init();
        adc_gpio_init(GpioPin);

        if constexpr (GpioPin == AdcPins::ADC_0) { _adc_number = 0; }
        if constexpr (GpioPin == AdcPins::ADC_1) { _adc_number = 1; }
        if constexpr (GpioPin == AdcPins::ADC_2) { _adc_number = 2; }
    }

    virtual float readVoltage() override
    {
        adc_select_input(_adc_number);
        uint16_t raw = adc_read();
        return raw * 3.3f / 4095.0f;
    }

private:

    uint8_t _adc_number;
};