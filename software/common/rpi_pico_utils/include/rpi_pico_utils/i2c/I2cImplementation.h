#pragma once

#include "../io/IoInterface.h"

#include <hardware/i2c.h>
#include <hardware/gpio.h>

class I2cImplementation : public IoInterface
{
public:

    struct Config
    {
        i2c_inst_t* i2c_inst;
        uint8_t scl_gpio;
        uint8_t sda_gpio;
        uint8_t addr;
        uint32_t baudrate;
        uint32_t rw_timeout_ms;
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

    virtual bool canWrite() override 
    { 
        // TODO
        return true; 
    }

    virtual bool read(uint8_t *data, uint32_t n_bytes_to_read, uint32_t& n_bytes_read) override
    {
        int ret = i2c_read_blocking_until(
            _conf.i2c_inst, 
            _conf.addr, 
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

    virtual bool write(const uint8_t *data, uint32_t n_bytes_to_write, uint32_t& n_bytes_written) override
    {
        int ret = i2c_write_blocking_until(
            _conf.i2c_inst, 
            _conf.addr, 
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

private:

    Config _conf;
};