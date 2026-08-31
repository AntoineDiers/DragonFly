#pragma once

#include "../io/IoInterface.h"

#include "UartBuffer.h"

#include <hardware/uart.h>
#include <hardware/irq.h>
#include <hardware/gpio.h>
#include <iostream>

enum class UartId
{
    UART_0 = 0,
    UART_1 = 1
};

class UartImplementation;

class UartSingleton
{
public:

    static void setUartHandler(const UartId& uart, UartImplementation* handler)
    {
        switch (uart)
        {
            case UartId::UART_0: { getInstance()._uart0_handler = handler; break; } 
            case UartId::UART_1: { getInstance()._uart1_handler = handler; break; }
        }
    }

    static void onTxInterrupt(const UartId& uart);
    static void onRxInterrupt(const UartId& uart);

private:

    static UartSingleton& getInstance()
    {
        static UartSingleton instance;
        return instance;
    }

    UartImplementation* _uart0_handler;

    UartImplementation* _uart1_handler;
};

void uart0_interrupt_handler() 
{ 
    uint32_t status = uart_get_hw(uart0)->mis;

    if (status & (UART_UARTMIS_RXMIS_BITS | UART_UARTMIS_RTMIS_BITS)) { UartSingleton::onRxInterrupt(UartId::UART_0); }
    if (status & UART_UARTMIS_TXMIS_BITS) { UartSingleton::onTxInterrupt(UartId::UART_0); }
}
void uart1_interrupt_handler() 
{
    uint32_t status = uart_get_hw(uart1)->mis;

    if (status & (UART_UARTMIS_RXMIS_BITS | UART_UARTMIS_RTMIS_BITS)) { UartSingleton::onRxInterrupt(UartId::UART_1); }
    if (status & UART_UARTMIS_TXMIS_BITS) { UartSingleton::onTxInterrupt(UartId::UART_1); }
}

class UartImplementation : public IoInterface
{
public:

    struct Config
    {
        UartId uart;
        uint32_t baudrate;
        uint8_t gpio_rx;
        uint8_t gpio_tx;
        uint8_t gpio_rts;
        uint8_t gpio_cts;
    };

    UartImplementation(const Config& config) : 
        _config(config)
    {
        irq_num_rp2350 irq = UART0_IRQ;
        irq_handler_t irq_handler = uart0_interrupt_handler;

        switch (_config.uart)
        {
            case UartId::UART_0:
            {
                _uart = uart0;
                irq = UART0_IRQ;
                irq_handler = uart0_interrupt_handler;
                break;
            }
            case UartId::UART_1:
            {
                _uart = uart1,
                irq = UART1_IRQ;
                irq_handler = uart1_interrupt_handler;
                break;
            }
        }

        gpio_set_function(_config.gpio_tx, GPIO_FUNC_UART);
        gpio_set_function(_config.gpio_rx, GPIO_FUNC_UART);
        gpio_set_function(_config.gpio_cts, GPIO_FUNC_UART);
        gpio_set_function(_config.gpio_rts, GPIO_FUNC_UART);

        uart_init(_uart, _config.baudrate);

        uart_set_format(_uart, 8, 1, UART_PARITY_NONE);

        uart_set_hw_flow(_uart, true, true);
        uart_set_fifo_enabled(_uart, true);

        irq_set_exclusive_handler(irq, irq_handler);
        uart_set_irq_enables(_uart, _interrupts_state.rx_enabled, _interrupts_state.tx_enabled);

        UartSingleton::setUartHandler(_config.uart, this);
        irq_set_enabled(irq, true);
    }

    virtual bool read(uint8_t *data, uint32_t n_bytes_to_read, uint32_t& n_bytes_read) override 
    {
        toggleRxInterrupt(false);
        n_bytes_read = _rx_buffer.read(data, n_bytes_to_read);
        toggleRxInterrupt(true);
        return true;
    }

    virtual bool write(const uint8_t *data, uint32_t n_bytes_to_write, uint32_t& n_bytes_written) override
    {
        toggleTxInterrupt(false);
        n_bytes_written = _tx_buffer.write(data, n_bytes_to_write);
        transmit();
        return true;
    }

    void receive()
    {
        while (uart_is_readable(_uart))
        {
            uint8_t byte = uart_getc(_uart);
            uint32_t n_bytes_written = _rx_buffer.write(&byte, 1);
            if(n_bytes_written == 0)
            {
                // TODO
            }
        }
    }
    
    void transmit()
    {
        while (uart_is_writable(_uart))
        {
            uint8_t byte;
            uint32_t n_bytes_read = _tx_buffer.read(&byte, 1);
            if(n_bytes_read == 0) 
            { 
                toggleTxInterrupt(false);
                return; 
            }
            uart_putc_raw(_uart, byte);
        }
        toggleTxInterrupt(true);
    }

private:

    struct InterruptsState
    {
        bool rx_enabled = true;
        bool tx_enabled = true;
    };

    void toggleTxInterrupt(bool active)
    {
        if(_interrupts_state.tx_enabled != active)
        {
            _interrupts_state.tx_enabled = active;
            uart_set_irq_enables(_uart, _interrupts_state.rx_enabled, _interrupts_state.tx_enabled);
        }
    }
    void toggleRxInterrupt(bool active)
    {
        if(_interrupts_state.rx_enabled != active)
        {
            _interrupts_state.rx_enabled = active;
            uart_set_irq_enables(_uart, _interrupts_state.rx_enabled, _interrupts_state.tx_enabled);
        }
    }

    uart_inst_t* _uart;
    Config _config;
    UartBuffer _rx_buffer;
    UartBuffer _tx_buffer;
    InterruptsState _interrupts_state;
};

inline void UartSingleton::onTxInterrupt(const UartId& uart) 
{
    UartImplementation* handler = nullptr;
    switch (uart)
    {
        case UartId::UART_0: { handler = getInstance()._uart0_handler; break; } 
        case UartId::UART_1: { handler = getInstance()._uart1_handler; break; }
    }
    if(handler){ handler->transmit(); }
}

inline void UartSingleton::onRxInterrupt(const UartId& uart) 
{
    UartImplementation* handler = nullptr;
    switch (uart)
    {
        case UartId::UART_0: { handler = getInstance()._uart0_handler; break; } 
        case UartId::UART_1: { handler = getInstance()._uart1_handler; break; }
    }
    if(handler){ handler->receive(); }
}