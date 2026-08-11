#pragma once

#include <cstdint>

template<typename T>
struct Stamped
{
    T value;
    uint64_t stamp_us = 0;

    uint64_t getElapsed_us(uint64_t now_us)
    {
        return now_us - stamp_us;
    }
};