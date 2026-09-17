#pragma once

#include "../io/IoInterface.h"

class SpiImplementation : public IoInterface
{
public:

    struct Config
    {
        uint8_t gpio_tx;
        uint8_t gpio_rx;
        uint8_t gpio_sck;
        uint8_t gpio_csn;
    };

    SpiImplementation(const Config& conf) : _conf(conf) {}

    virtual bool read(uint8_t *data, uint32_t n_bytes_to_read, uint32_t& n_bytes_read) override 
    { 
        (void)data;
        (void)n_bytes_to_read;
        (void)n_bytes_read;
        return false; 
    };
    virtual bool write(const uint8_t *data, uint32_t n_bytes_to_write, uint32_t& n_bytes_written) override 
    { 
        (void)data;
        (void)n_bytes_to_write;
        (void)n_bytes_written;
        return false; 
    };

private:

    Config _conf;
};