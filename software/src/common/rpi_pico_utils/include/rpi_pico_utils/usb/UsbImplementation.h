#pragma once

#include "../io/IoInterface.h"

class UsbImplementation : public IoInterface
{
public:

    UsbImplementation()
    {
        // TODO
    }

    virtual bool read(uint8_t *data, uint32_t n_bytes_to_read, uint32_t& n_bytes_read) override 
    { 
        (void)data;
        (void)n_bytes_to_read;
        (void)n_bytes_read;

        // TODO
        return false; 
    }

    virtual bool write(const uint8_t *data, uint32_t n_bytes_to_write, uint32_t& n_bytes_written) override
    {
        (void)data;
        (void)n_bytes_to_write;
        (void)n_bytes_written;

        // TODO
        return false; 
    }
};