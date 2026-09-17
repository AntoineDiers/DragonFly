#pragma once

#include <cstdint>

#include <dragonfly_utils/Buffer.h>
#include <dragonfly_utils/Event.h>
#include <optional>
#include <cstring>
#include <cstdint>

#include <dragonfly_msgs/dragonfly_msgs.h>

#include <iostream>
#include <variant>

#include "Peer.h"


/*
    Available Bandwidth : 5 kbps

    Ping Packet : 

        PingMagic           6 bits
        Src                 2 bits
        Sequence            8 bits
        CRC                 8 bits

        Total           3 bytes
        Rate            10Hz

    Pong Packet : 

        PongMagic           4 bits
        Src                 2 bits
        Dest                2 bits
        Sequence            8 bits
        CRC                 8 bits

        Total           3 bytes
        Rate            10Hz

    Short Datagram Packet :

        ShortDatagramMagic      7 bits
        DatagramSize            5 bits
        Src                     2 bits
        Dst                     2 bits
        Datagram                0 -> 31 bytes
        CRC_8                   8 bits

    Long Datagram Packet :

        LongDatagramMagic1      8 bits
        LongDatagramMagic2      8 bits
        DatagramSize            12 bits
        Src                     2 bits
        Dst                     2 bits
        Datagram                0 -> MAX_DATAGRAM_SIZE bytes
        CRC_16                  16 bits
*/

namespace dragonfly_datagram_protocol
{
    static constexpr uint8_t PING_MAGIC =               0b11000100;
    static constexpr uint8_t PONG_MAGIC =               0b11010000;
    static constexpr uint8_t SHORT_DATAGRAM_MAGIC =     0b10110010;
    static constexpr uint8_t LONG_DATAGRAM_MAGIC_1 =    0b01010010;
    static constexpr uint8_t LONG_DATAGRAM_MAGIC_2 =    0b01010011;

    static constexpr uint32_t PING_PACKET_SIZE = 3;
    static constexpr uint32_t PONG_PACKET_SIZE = 3;

    static constexpr uint32_t SHORT_DATAGRAM_MAX_SIZE = 31;
    static constexpr uint32_t SHORT_DATAGRAM_OVERHEAD = 3;
    static constexpr uint32_t SHORT_DATAGRAM_PACKET_SIZE = SHORT_DATAGRAM_MAX_SIZE + SHORT_DATAGRAM_OVERHEAD;

    static constexpr uint32_t LONG_DATAGRAM_MAX_SIZE = std::max(dragonfly_msgs::MAX_MSG_SIZE, uint32_t(31));
    static constexpr uint32_t LONG_DATAGRAM_OVERHEAD = 6;
    static constexpr uint32_t LONG_DATAGRAM_PACKET_SIZE = LONG_DATAGRAM_MAX_SIZE + LONG_DATAGRAM_OVERHEAD;

    static constexpr uint32_t MAX_DATAGRAM_SIZE = LONG_DATAGRAM_MAX_SIZE;
    static constexpr uint32_t MAX_PACKET_SIZE = LONG_DATAGRAM_PACKET_SIZE;

    struct PingPacket
    {
        PeerId src;
        uint8_t sequence;
    };

    struct PongPacket
    {
        PeerId src;
        PeerId dst;
        uint8_t sequence;
    };

    struct DatagramPacket
    {
        PeerId src;
        PeerId dst;
        Buffer<MAX_DATAGRAM_SIZE> data;
    };

    typedef Buffer<MAX_PACKET_SIZE> PacketData;

    typedef std::variant<PingPacket, PongPacket, DatagramPacket> Packet;

    void crc8Step(uint8_t& crc, uint8_t data)
    {
        crc ^= data;

        for (uint8_t bit = 0; bit < 8; ++bit)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }

    uint8_t crc8(const uint8_t* data, uint32_t size)
    {
        uint8_t crc = 0x00;
        for (uint32_t i = 0; i < size; ++i) { crc8Step(crc, data[i]); }
        return crc;
    }

    void crc16Step(uint16_t& crc, uint8_t data)
    {
        crc ^= static_cast<uint16_t>(data) << 8;

        for (int bit = 0; bit < 8; ++bit)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }

    uint16_t crc16(const uint8_t* data, uint32_t length)
    {
        uint16_t crc = 0xFFFF;
        for (size_t i = 0; i < length; ++i) { crc16Step(crc, data[i]); }
        return crc;
    }


    PacketData serialisePing(const PingPacket& ping)
    {
        PacketData res;
        res.size = PING_PACKET_SIZE;
        res.data[0] = PING_MAGIC + (uint8_t)ping.src;
        res.data[1] = ping.sequence;
        res.data[2] = crc8(res.data.data(), res.size - 1);

        return res;
    }

    PacketData serialisePong(const PongPacket& pong)
    {
        PacketData res;
        res.size = PONG_PACKET_SIZE;
        res.data[0] = PONG_MAGIC + ((uint8_t)pong.src << 2) + (uint8_t)pong.dst;
        res.data[1] = pong.sequence;
        res.data[2] = crc8(res.data.data(), res.size - 1);

        return res;
    }

    PacketData serialiseDatagram(const DatagramPacket& datagram)
    {
        PacketData res;

        if(datagram.data.size <= SHORT_DATAGRAM_MAX_SIZE)
        {
            res.size = datagram.data.size + SHORT_DATAGRAM_OVERHEAD;

            res.data[0] = SHORT_DATAGRAM_MAGIC + (datagram.data.size >> 4);
            res.data[1] = ((datagram.data.size & 0x0F) << 4) + ((uint8_t)datagram.src << 2) + (uint8_t)datagram.dst;
            std::memmove(res.data.data() + 2, datagram.data.data.data(), datagram.data.size);
            res.data[res.size - 1] = crc8(res.data.data(), res.size - 1);
        }
        else
        {
            res.size = datagram.data.size + LONG_DATAGRAM_OVERHEAD;

            res.data[0] = LONG_DATAGRAM_MAGIC_1;
            res.data[1] = LONG_DATAGRAM_MAGIC_2;
            res.data[2] = datagram.data.size >> 4;
            res.data[3] = ((datagram.data.size & 0x0F) << 4) + ((uint8_t)datagram.src << 2) + (uint8_t)datagram.dst;
            std::memmove(res.data.data() + 4, datagram.data.data.data(), datagram.data.size);

            uint16_t crc_16 = crc16(res.data.data(), res.size - 2);
            res.data[res.size - 2] = (crc_16 >> 8) & 0xFF;
            res.data[res.size - 1] = crc_16 & 0xFF;
        }

        return res;
    }

    class Deserialiser
    {
    public:

        Deserialiser(Event* corrupted_data_error) : 
            _corrupted_data_error(corrupted_data_error) {}

        std::optional<Packet> feed(uint8_t byte)
        {
            _buffer.pushBack(byte);
            
            if((_buffer[0] & 0b11111100) == PING_MAGIC)
            {
                // Check we have enough data
                if(_buffer.size() < PING_PACKET_SIZE) 
                { 
                    // Not enough data received, retry later
                    return std::nullopt; 
                }

                // Check the data crc
                if(_buffer.checkCrc8(PING_PACKET_SIZE - 1))
                {
                    // Extract the data
                    PingPacket res 
                    {
                        .src = (PeerId)(_buffer[0] & 0b00000011), 
                        .sequence = _buffer[1]
                    };

                    _buffer.popFront(PING_PACKET_SIZE);
                    return res;
                }
            }

            else if((_buffer[0] & 0b11110000) == PONG_MAGIC)
            {
                // Check we have enough data
                if(_buffer.size() < PONG_PACKET_SIZE) 
                { 
                    // Not enough data received, retry later
                    return std::nullopt; 
                }

                // Check the data crc
                if(_buffer.checkCrc8(PONG_PACKET_SIZE - 1))
                {
                    // Extract the data
                    PongPacket res 
                    {
                        .src = (PeerId)((_buffer[0] >> 2) & 0b00000011),
                        .dst = (PeerId)(_buffer[0] & 0b00000011),
                        .sequence = _buffer[1]
                    };

                    _buffer.popFront(PONG_PACKET_SIZE);
                    return res;
                }
            }

            else if((_buffer[0]& 0b11111110) == SHORT_DATAGRAM_MAGIC )
            {
                // Check we have enough data
                if(_buffer.size() < SHORT_DATAGRAM_OVERHEAD) 
                { 
                    // Not enough data received, retry later
                    return std::nullopt; 
                }

                uint32_t datagram_size = ((_buffer[0] & 0b00000001) << 4) + (_buffer[1] >> 4);
                uint32_t packet_size = datagram_size + SHORT_DATAGRAM_OVERHEAD;
                if(_buffer.size() < datagram_size + SHORT_DATAGRAM_OVERHEAD) 
                {
                    // Not enough data received, retry later
                    return std::nullopt; 
                }

                if(_buffer.checkCrc8(packet_size - 1))
                {
                    DatagramPacket res;
                    res.src = (PeerId)((_buffer[1] >> 2) & 0b00000011);
                    res.dst = (PeerId)(_buffer[1] & 0b00000011);
                    res.data.size = datagram_size;
                    _buffer.copy(res.data.data.data(), 2, datagram_size);
                    _buffer.popFront(packet_size);
                    return res;
                }
            }

            else if(_buffer[0] == LONG_DATAGRAM_MAGIC_1)
            {
                if(_buffer.size() == 1) 
                {
                    // Not enough data received, retry later 
                    return std::nullopt; 
                }

                if(_buffer[1] == LONG_DATAGRAM_MAGIC_2)
                {
                    // Check we have enough data
                    if(_buffer.size() < LONG_DATAGRAM_OVERHEAD) 
                    { 
                        // Not enough data received, retry later
                        return std::nullopt; 
                    }

                    uint32_t datagram_size = (_buffer[2] << 4) + (_buffer[3] >> 4);
                    uint32_t packet_size = datagram_size + LONG_DATAGRAM_OVERHEAD;

                    if(datagram_size <= LONG_DATAGRAM_MAX_SIZE)
                    {
                        if(_buffer.size() < datagram_size + LONG_DATAGRAM_OVERHEAD) 
                        {
                            // Not enough data received, retry later
                            return std::nullopt; 
                        }

                        if( _buffer.checkCrc16(packet_size - 2))
                        {
                            DatagramPacket res;
                            res.src = (PeerId)((_buffer[3] >> 2) & 0b00000011);
                            res.dst = (PeerId)(_buffer[3] & 0b00000011);
                            _buffer.copy(res.data.data.data(), 4, datagram_size);
                            _buffer.popFront(packet_size);
                            return res;
                        }
                    }                 
                }
            }

            // We only arrive down here if invalid data was found, in that case, we flush the front 
            // byte of the buffer and go to the next magic number
            _corrupted_data_error->trigger();
            _buffer.popFront(1);
            while(  _buffer.size() > 0 && 
                    (_buffer[0] & 0b11111100) != PING_MAGIC && 
                    (_buffer[0] & 0b11111100) != PONG_MAGIC && 
                    (_buffer[0] & 0b11111110) != SHORT_DATAGRAM_MAGIC && 
                    _buffer[0] != LONG_DATAGRAM_MAGIC_1)
            {
                _buffer.popFront(1);
            }

            return std::nullopt;
        }

    private:

        class CircularBuffer
        {
        public:

            void pushBack(uint8_t val)
            {
                uint32_t write_index = (_start_index + _size) % MAX_PACKET_SIZE;
                _data[write_index] = val;
                if(_size < MAX_PACKET_SIZE) { _size++; }
                else { _start_index++; }
            }
            void popFront(uint32_t n)
            {
                _start_index = (_start_index + n) % MAX_PACKET_SIZE;
                _size -= n;
            }
            uint8_t operator[](const uint32_t& pos)
            {
                return _data[(_start_index + pos) % MAX_PACKET_SIZE];
            }
            uint32_t size()
            {
                return _size;
            }
            void copy(uint8_t* dst, uint32_t start_index, uint32_t size)
            {
                start_index = (_start_index + start_index) % MAX_PACKET_SIZE;
                
                // TODO : optimize
                for(uint32_t i = 0; i < size; i++)
                {
                    *(dst + i) = _data[(start_index + i) % MAX_PACKET_SIZE];
                }
            }

            bool checkCrc8(uint32_t crc_index)
            {
                uint8_t expected_crc = (*this)[crc_index];
                uint8_t actual_crc = 0;
                for(uint32_t i = 0; i < crc_index; i++)
                {
                    crc8Step(actual_crc, (*this)[i]);
                }

                if(expected_crc != actual_crc)
                {
                    std::cout << "BAD CRC, expected " << (int)actual_crc << ", got " << (int)expected_crc << "\n";
                }

                return expected_crc == actual_crc;
            }

            bool checkCrc16(uint32_t crc_index)
            {
                uint16_t expected_crc = ((uint16_t)(this->operator[](crc_index)) << 8) + this->operator[](crc_index + 1);
                uint16_t actual_crc = 0;
                for(uint32_t i = 0; i < crc_index; i++)
                {
                    crc16Step(actual_crc, this->operator[](i));
                }
                return expected_crc == actual_crc;
            }

        private:

            uint8_t _data[MAX_PACKET_SIZE];
            uint32_t _start_index = 0;
            uint32_t _size = 0;
        };

        Event* _corrupted_data_error;
        CircularBuffer _buffer;
    };
}