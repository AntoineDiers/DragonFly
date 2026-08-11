#pragma once

#include "../io/IoInterface.h"

class UartImplementation : public IoInterface
{
public:

    struct Config
    {
        uint8_t GPIO_RX;
        uint8_t GPIO_TX;
        uint8_t GPIO_RTS;
        uint8_t GPIO_CTS;  
    };

    UartImplementation(const Config& config) : 
        _config(config)
    {
        // TODO
    }

    virtual bool canWrite() override 
    { 
        // TODO
        return true; 
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

private:

    Config _config;
};