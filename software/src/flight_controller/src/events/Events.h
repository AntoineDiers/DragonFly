#pragma once

#include <dragonfly_datagram_protocol/Multiplexer.h>

struct FlightControllerEvents
{
    Event rf_link_rw_error;
    Event rf_link_rx_corrupted_data;
    Event rf_link_tx_data_discarded;
    Event rf_link_tx_unexpectedly_big_packet;

    Event spi_link_rw_error;
    Event spi_link_rx_corrupted_data;
    Event spi_link_tx_data_discarded;
    Event spi_link_tx_unexpectedly_big_packet;

    Event unhandled_datagram;

    FlightControllerEvents(ClockInterface* clock) : 
        rf_link_rw_error(                   clock, "RF link : R/W Error",                               1000, Event::Criticity::ERROR),
        rf_link_rx_corrupted_data(          clock, "RF link : Corrupted data received",                 1000, Event::Criticity::WARNING),
        rf_link_tx_data_discarded(          clock, "RF link : Data discarded from Tx buffer",           1000, Event::Criticity::WARNING),
        rf_link_tx_unexpectedly_big_packet( clock, "RF link : Unexpectedly big packet in Tx buffer",    1000, Event::Criticity::ERROR),

        spi_link_rw_error(                  clock, "SPI link : R/W Error",                              1000, Event::Criticity::ERROR),
        spi_link_rx_corrupted_data(         clock, "SPI link : Corrupted data received",                1000, Event::Criticity::WARNING),
        spi_link_tx_data_discarded(         clock, "SPI link : Data discarded from Tx buffer",          1000, Event::Criticity::WARNING),
        spi_link_tx_unexpectedly_big_packet(clock, "SPI link : Unexpectedly big packet in Tx buffer",   1000, Event::Criticity::ERROR),

        unhandled_datagram(                 clock, "Unhandled datagram",                                1000, Event::Criticity::ERROR)
    {}

    dragonfly_datagram_protocol::Multiplexer::Events getMultiplexerEvents()
    {
        return dragonfly_datagram_protocol::Multiplexer::Events
        {
            .remote_link = 
            {
                .tx_buffer_events = 
                {
                    .unexpectedly_big_packet = &rf_link_tx_data_discarded,
                    .data_discarded = &rf_link_tx_data_discarded,
                },
                .rw_error = &spi_link_rw_error,
                .corrupted_rx_data = &rf_link_rx_corrupted_data,
            },
            .local_link = 
            {
                .tx_buffer_events = 
                {
                    .unexpectedly_big_packet = &spi_link_tx_data_discarded,
                    .data_discarded = &spi_link_tx_data_discarded,
                },
                .rw_error = &spi_link_rw_error,
                .corrupted_rx_data = &spi_link_rx_corrupted_data,
            },
        };
    }

    dragonfly_msgs::msgs::flight_controller::Errors toMsg()
    {
        return dragonfly_msgs::msgs::flight_controller::Errors
        {
            .rf_link_rw_error =                    rf_link_rw_error.getState(),
            .rf_link_corrupted_data =              rf_link_rx_corrupted_data.getState(),
            .rf_link_tx_data_discarded =           rf_link_tx_data_discarded.getState(),
            .rf_link_tx_unexpectedly_big_packet =  rf_link_tx_unexpectedly_big_packet.getState(),

            .spi_link_rw_error =                    spi_link_rw_error.getState(),
            .spi_link_corrupted_data =              spi_link_rx_corrupted_data.getState(),
            .spi_link_tx_data_discarded =           spi_link_tx_data_discarded.getState(),
            .spi_link_tx_unexpectedly_big_packet =  spi_link_tx_unexpectedly_big_packet.getState(),
        };
    }
};