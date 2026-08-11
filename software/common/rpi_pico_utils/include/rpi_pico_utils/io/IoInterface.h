#pragma once

#include <cstdint>

class IoInterface
{
public:

    virtual bool read(uint8_t *data, uint32_t n_bytes_to_read, uint32_t& n_bytes_read) = 0;
    virtual bool canWrite() = 0;
    virtual bool write(const uint8_t *data, uint32_t n_bytes_to_write, uint32_t& n_bytes_written) = 0;
};