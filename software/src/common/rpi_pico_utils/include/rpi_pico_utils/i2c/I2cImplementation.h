#pragma once

#include "../io/IoInterface.h"

#include <hardware/i2c.h>
#include <hardware/gpio.h>

class I2cImplementation
{
public:

    struct Config
    {
        i2c_inst_t* i2c_inst;
        uint8_t scl_gpio;
        uint8_t sda_gpio;
        uint32_t baudrate;
        uint32_t rw_timeout_ms;
    };
    
    class SlaveHandle : public IoInterface
    {
    public:
        SlaveHandle(I2cImplementation* impl, uint8_t slave_addr) : 
            _impl(impl),
            _slave_addr(slave_addr)
        {}

        virtual bool read(uint8_t *data, uint32_t n_bytes_to_read, uint32_t& n_bytes_read) override
        {
            return _impl->read(_slave_addr, data, n_bytes_to_read, n_bytes_read);
        }

        virtual bool write(const uint8_t *data, uint32_t n_bytes_to_write, uint32_t& n_bytes_written) override
        {
            return _impl->write(_slave_addr, data, n_bytes_to_write, n_bytes_written);
        }

    private:
        
        I2cImplementation* _impl;
        uint8_t _slave_addr;
        
    };

    I2cImplementation(const Config& conf) : 
        _conf(conf)
    {
        i2c_init(_conf.i2c_inst, _conf.baudrate);
        gpio_set_function(_conf.sda_gpio, GPIO_FUNC_I2C);
        gpio_set_function(_conf.scl_gpio, GPIO_FUNC_I2C);
        gpio_pull_up(_conf.sda_gpio);
        gpio_pull_up(_conf.scl_gpio);
    }

    SlaveHandle generateSlaveHandle(uint8_t slave_addr)
    {
        return SlaveHandle(this, slave_addr);
    }

private:

    bool read(uint8_t slave_addr, uint8_t *data, uint32_t n_bytes_to_read, uint32_t& n_bytes_read)
    {
        int ret = i2c_read_blocking_until(
            _conf.i2c_inst, 
            slave_addr, 
            data, 
            n_bytes_to_read, 
            false, 
            make_timeout_time_ms(_conf.rw_timeout_ms));
        
        if(ret >= 0)
        {
            n_bytes_read = ret;
            return true;
        }
        return false;
    };

    bool write(uint8_t slave_addr, const uint8_t *data, uint32_t n_bytes_to_write, uint32_t& n_bytes_written)
    {
        int ret = i2c_write_blocking_until(
            _conf.i2c_inst, 
            slave_addr, 
            data, 
            n_bytes_to_write,
            false,
            make_timeout_time_ms(_conf.rw_timeout_ms));

        if(ret >= 0)
        {
            n_bytes_written = ret;
            return true;
        }
        return false;
    }

    Config _conf;
};