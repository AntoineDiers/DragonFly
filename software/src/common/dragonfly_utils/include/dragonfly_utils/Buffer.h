#pragma once

#include <array>
#include <cstdint>

template<uint32_t Size>
struct Buffer
{
    std::array<uint8_t, Size> data;
    uint32_t size = 0;
};