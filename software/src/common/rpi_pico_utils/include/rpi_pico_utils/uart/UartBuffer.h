#pragma once

#include <array>
#include <cstdint>
#include <cstring>

#include <config/common/comms/Config.h>

class UartBuffer
{
public:

    uint32_t write(const uint8_t* data, uint32_t size)
    {
        if(_size >= Capacity || size == 0) { return 0; }

        uint32_t write_size = std::min(size, Capacity - _size);
        uint32_t write_index = (_index + _size) % Capacity;

        if(write_index + write_size <= Capacity)
        {
            std::memcpy(_buffer.data() + write_index, data, write_size);
        }
        else
        {   // Wrap
            uint32_t first_write_size = Capacity - write_index;
            uint32_t second_write_size = write_size - first_write_size;
            std::memcpy(_buffer.data() + write_index, data, first_write_size);
            std::memcpy(_buffer.data(), data + first_write_size, second_write_size);
        }

        _size += write_size;
        return write_size;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"

    uint32_t read(uint8_t* data, uint32_t size)
    {
        if(_size == 0 || size == 0) { return 0; }

        uint32_t read_size = std::min(size, _size);

        if(_index + read_size <= Capacity)
        {
            std::memcpy(data, _buffer.data() + _index, read_size);
        }
        else
        {   // Wrap
            uint32_t first_read_size = Capacity - _index;
            uint32_t second_read_size = read_size - first_read_size;
            std::memcpy(data, _buffer.data() + _index , first_read_size);
            std::memcpy(data + first_read_size, _buffer.data(), second_read_size);
        }

        _index = (_index + read_size) % Capacity;
        _size -= read_size;
        return read_size;
    }

#pragma GCC diagnostic pop

private:

    static constexpr uint32_t Capacity = config::common::comms::UART_BUFFER_SIZE;

    std::array<uint8_t, Capacity> _buffer;
    uint32_t _index = 0;
    uint32_t _size = 0;
};