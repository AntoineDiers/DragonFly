#pragma once

#include <dragonfly_datagram_protocol/Multiplexer.h>

#include <config/remote_control/inputs/Config.h>
#include <config/remote_control/leds/Config.h>
#include <config/remote_control/diagnostics/Config.h>

#include <dragonfly_msgs/msg/remote_control/Diagnostics.h>

#include "status_led/StatusLed.h"
#include "inputs/Inputs.h"

#include <iostream>

class RemoteControl
{
public:
  
    RemoteControl(const HardwareInterface& hw_interface) : 
        _hw_interface(hw_interface),
        _comms_multiplexer(hw_interface.clock, hw_interface.gs_usb, hw_interface.rf_uart, PeerId::REMOTE_CONTROL),
        _inputs(hw_interface),
        _send_inputs_timer(hw_interface.clock, config::remote_control::inputs::SEND_INPUTS_PERIOD_MS),
        _send_diagnostics_timer(hw_interface.clock, config::remote_control::diags::SEND_PERIOD_MS),
        _pico_led_blink_timer(hw_interface.clock, config::remote_control::leds::LED_BLINK_PERIOD_MS),
        _status_led(hw_interface.clock, hw_interface.status_led_r, hw_interface.status_led_g),
        _link_led(hw_interface.clock, hw_interface.link_led_r, hw_interface.link_led_g)
    {};

    void tick()
    {
        _hw_interface.watchdog->update();
        
        // -------------------------
        // Poll incoming messages
        // -------------------------

        dragonfly_datagram_protocol::Multiplexer::DatagramPair incoming_datagrams = _comms_multiplexer.poll();
        if(incoming_datagrams.local_link_datagram) { onMessage(incoming_datagrams.local_link_datagram.value()); }
        if(incoming_datagrams.remote_link_datagram) { onMessage(incoming_datagrams.remote_link_datagram.value()); }

        // -------------------------
        // Update and send inputs when necessary
        // -------------------------

        _inputs.tick();
        if(_send_inputs_timer.poll())
        {
            bool inputs_error;
            dragonfly_msgs::msg::remote_control::Inputs inputs = _inputs.read(inputs_error);
            _comms_multiplexer.sendMessage(inputs, PeerId::FLIGHT_CONTROLLER);
            
            _status_led.setStatus(inputs_error ? StatusLed::Status::WARN : StatusLed::Status::OK);
        }

        // -------------------------
        // Send diagnostics when necessary
        // -------------------------

        if(_send_diagnostics_timer.poll())
        {
            dragonfly_msgs::msg::remote_control::Diagnostics diags
            {
                .connections_status = _comms_multiplexer.getConnectionsStatus(),
                .inputs_diagnostics = _inputs.getDiagnostics()
            };
            _comms_multiplexer.sendMessage(diags, PeerId::GROUND_COMPUTER);
        }

        // -------------------------
        // Update leds
        // -------------------------

        if(_pico_led_blink_timer.poll())
        {
            _pico_led_state = !_pico_led_state;
            _hw_interface.pico_led->set(_pico_led_state);
        }

        // Status led
        _status_led.tick();

        // Link led

        // TODO
        /*if( flight_controller_link_stats.packet_loss_percent > config::remote_control::leds::ERROR_PACKET_LOSS_PERCENT || 
            flight_controller_link_stats.ping_ms > config::remote_control::leds::ERROR_PING_MS)
        {
            _link_led.setStatus(StatusLed::Status::ERR);
        }
        else if(flight_controller_link_stats.packet_loss_percent > config::remote_control::leds::WARN_PACKET_LOSS_PERCENT || 
                flight_controller_link_stats.ping_ms > config::remote_control::leds::WARN_PING_MS)
        {
            _link_led.setStatus(StatusLed::Status::WARN);
        }
        else
        {
            _link_led.setStatus(StatusLed::Status::OK);
        }
        _link_led.tick();*/
    }

private:

    void onMessage(const dragonfly_datagram_protocol::DatagramPacket& incoming_datagram)
    {
        (void) incoming_datagram;
        std::cout << "Got datagram ! \n"; 
    }

    HardwareInterface _hw_interface;

    dragonfly_datagram_protocol::Multiplexer _comms_multiplexer;

    Inputs _inputs;
    Timer _send_inputs_timer;
    Timer _send_diagnostics_timer;
    Timer _pico_led_blink_timer;
    bool _pico_led_state;

    StatusLed _status_led;
    StatusLed _link_led;
};