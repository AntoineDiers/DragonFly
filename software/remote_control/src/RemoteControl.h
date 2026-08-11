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
        _comms_multiplexer(hw_interface.clock, hw_interface.gs_usb, hw_interface.rf_uart),
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
        // Update and send inputs when necessary
        // -------------------------

        _inputs.tick();
        if(_send_inputs_timer.poll())
        {
            bool inputs_error;
            _comms_multiplexer.send<dragonfly_datagram_protocol::Node::FLIGHT_CONTROLLER>(_inputs.read(inputs_error));

            _status_led.setStatus(inputs_error ? StatusLed::Status::WARN : StatusLed::Status::OK);
        }

        // -------------------------
        // Send diagnostics when necessary
        // -------------------------

        if(_send_diagnostics_timer.poll())
        {
            _comms_multiplexer.send<dragonfly_datagram_protocol::Node::GROUND_STATION>(generateDiagnostics());
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
        dragonfly_msgs::msg::link_stats::LinkStats flight_controller_link_stats = 
            _comms_multiplexer.getLinkStats<dragonfly_datagram_protocol::Node::FLIGHT_CONTROLLER>();

        if( flight_controller_link_stats.packet_loss_percent > config::remote_control::leds::ERROR_PACKET_LOSS_PERCENT || 
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
        _link_led.tick();
    }

private:

    dragonfly_msgs::msg::remote_control::Diagnostics generateDiagnostics()
    {
        auto links_diags = _comms_multiplexer.getFullLinkStats();

        dragonfly_msgs::msg::remote_control::Diagnostics res;
        res.n_restarts = _hw_interface.watchdog->restartsCount();

        res.links_diagnostics.gs = links_diags.local_peer;
        res.links_diagnostics.fc_via_rf = links_diags.direct_remote_peer_via_remote_link;
        res.links_diagnostics.fc_via_4g = links_diags.direct_remote_peer_via_local_link;
        res.links_diagnostics.pc_via_rf = links_diags.indirect_remote_peer_via_remote_link;
        res.links_diagnostics.pc_via_4g = links_diags.indirect_remote_peer_via_local_link;

        res.inputs_diagnostics = _inputs.getDiagnostics();

        std::cout << res << "\n";
        return res;
    }

    HardwareInterface _hw_interface;

    dragonfly_datagram_protocol::Multiplexer<
        dragonfly_datagram_protocol::Node::REMOTE_CONTROL,
        dragonfly_datagram_protocol::Node::GROUND_STATION,
        dragonfly_datagram_protocol::Node::FLIGHT_CONTROLLER,
        dragonfly_datagram_protocol::Node::PAYLOAD_COMPUTER> _comms_multiplexer;

    Inputs _inputs;
    Timer _send_inputs_timer;
    Timer _send_diagnostics_timer;
    Timer _pico_led_blink_timer;
    bool _pico_led_state;

    StatusLed _status_led;
    StatusLed _link_led;

    uint8_t _count = 5;
};