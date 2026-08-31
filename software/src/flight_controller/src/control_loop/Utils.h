#pragma once

#include <cstdint>

inline float rcInputToFloat(uint8_t input)
{
    return input / 255.0;
};

inline float centeredRcInputToFloat(uint8_t input)
{
    if(input > 127)
    {
        return ((float)(input - 127)) / (255 - 127);
    }
    else if(input < 127)
    {
        return - ((float)(127 - input)) / 127;
    }
    else
    {
        return 0.0f;
    }
}