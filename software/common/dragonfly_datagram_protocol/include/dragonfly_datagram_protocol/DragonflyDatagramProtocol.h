#pragma once

#include <cstdint>

#include <dragonfly_utils/Buffer.h>
#include <optional>
#include <cstring>

/*
    | Field     | Size (bytes)  | Description
    |-----------|---------------|---------------
    | MAGIC 'D' | 1             | The 'D' character (0x44)
    | MAGIC 'P' | 1             | The 'P' character (0x50)
    | Src / Dst | 1             | Least significant 4 bits : source of the packet, most significant 4 bits : destinations of the packet (bitmap)
    | Size      | 1             | The packet size in bytes
    | Sequence  | 1             | Least significant 4 bits : last sequence index, most significant 4 bits : index in sequence (starting at 0)
    | Payload   | Size          | The packet payload
    | CRC-16    | 2             | CRC-16 checksum (calculated on the payload, including previous packets from the sequence)
*/

namespace dragonfly_datagram_protocol
{
    static constexpr uint8_t MAX_PACKET_SIZE = 255;
    static constexpr uint8_t HEADER_SIZE = 5;
    static constexpr uint8_t CRC_SIZE = 2;
    static constexpr uint8_t PACKET_OVERHEAD_BYTES = HEADER_SIZE + CRC_SIZE;
    static constexpr uint8_t MAX_PAYLOAD_SIZE_PER_PACKET = MAX_PACKET_SIZE - PACKET_OVERHEAD_BYTES;
    static constexpr uint8_t MAX_FRAGMENTS = 4;
    static constexpr uint32_t MAX_PAYLOAD_SIZE = MAX_PAYLOAD_SIZE_PER_PACKET * MAX_FRAGMENTS;
    static constexpr uint32_t MAX_SIZE_TOTAL = MAX_PACKET_SIZE * MAX_FRAGMENTS;
    static constexpr uint8_t TX_BUFFER_SIZE_PACKETS = 20;

    static_assert(TX_BUFFER_SIZE_PACKETS >= 2 * MAX_FRAGMENTS);

    enum Node
    {
        GROUND_STATION = 0,
        REMOTE_CONTROL,
        FLIGHT_CONTROLLER,
        PAYLOAD_COMPUTER
    };

    void CRC_16(const uint8_t* data, uint32_t size, uint8_t& crc_0, uint8_t& crc_1)
    {
        uint16_t crc = 0xFFFF;

        while (size--)
        {
            crc ^= static_cast<uint16_t>(*data++) << 8;

            for (uint8_t i = 0; i < 8; ++i)
            {
                if (crc & 0x8000)
                    crc = (crc << 1) ^ 0x1021;
                else
                    crc <<= 1;
            }
        }

        crc_0 = static_cast<uint8_t>((crc >> 8) & 0xFF); // MSB
        crc_1 = static_cast<uint8_t>(crc & 0xFF);        // LSB
    }

    class Serialiser
    {
    public:

        template<uint32_t PayloadSize>
        void feedDatagram(const Buffer<PayloadSize>& payload, Node from, uint8_t to)
        {
            static_assert(PayloadSize <= MAX_PAYLOAD_SIZE);

            uint32_t n_packets = payload.size / MAX_PAYLOAD_SIZE_PER_PACKET;

            for(uint8_t sequence = 0; sequence < n_packets; sequence++)
            {
                Buffer<MAX_PACKET_SIZE> packet;

                uint32_t payload_start = sequence * MAX_PAYLOAD_SIZE_PER_PACKET;
                uint8_t packet_size = std::min((uint32_t)MAX_PACKET_SIZE, PACKET_OVERHEAD_BYTES + payload.size - payload_start);
                uint8_t payload_fragment_size = packet_size - PACKET_OVERHEAD_BYTES;

                packet.data[0] = 'D';
                packet.data[1] = 'P';
                packet.data[2] = (uint8_t)from + ((uint8_t)to << 4);
                packet.data[3] = packet_size;
                packet.data[4] = sequence;

                std::memcpy(packet.data.data() + 5, payload.data.data() + payload_start, payload_fragment_size);

                uint8_t crc_0, crc_1;
                CRC_16(payload.data.data(), payload_start + payload_fragment_size, crc_0, crc_1);
                packet.data[packet_size - 2] = crc_0;
                packet.data[packet_size - 1] = crc_1;

                packet.size = packet_size;

                bool is_datagram_start = sequence == 0;

                _buffer.addPacket(packet, is_datagram_start);
            }
        }

        std::optional<Buffer<MAX_PACKET_SIZE>> getNextPacket()
        {
            return _buffer.getNextPacket();
        }

    private:

        class PacketBuffer
        {
        public:
            
            void addPacket(const Buffer<MAX_PACKET_SIZE>& packet, bool is_datagram_start)
            {
                if(_size >= TX_BUFFER_SIZE_PACKETS)
                {
                    removeOldestNonPendingDatagram();
                }

                if(_size >= TX_BUFFER_SIZE_PACKETS) { return; } // Should never happen

                _packets_buffer[_size] = packet;
                _datagram_starts[_size] = is_datagram_start;
                _size++;
            }

            std::optional<Buffer<MAX_PACKET_SIZE>> getNextPacket()
            {
                if(_size > 0)
                {
                    Buffer<MAX_PACKET_SIZE> res = _packets_buffer[0];
                    _size--;
                    std::memmove(_packets_buffer, _packets_buffer + 1, _size * sizeof(Buffer<MAX_PACKET_SIZE>));
                    return res;
                }
                return std::nullopt;
            }

        private:

            void removeOldestNonPendingDatagram()
            {
                for(uint32_t start = 0; start < _size; start++)
                {
                    if(_datagram_starts[start])
                    {
                        for(uint32_t end = start + 1; end < _size; end++)
                        {
                            if(_datagram_starts[end])
                            {
                                std::memmove(_packets_buffer + start, _packets_buffer + end, (end - start) * sizeof(Buffer<MAX_PACKET_SIZE>));
                                _size -= (end - start);
                            }
                        }
                    }
                }
            }
            
            Buffer<MAX_PACKET_SIZE> _packets_buffer[TX_BUFFER_SIZE_PACKETS];
            uint32_t _size = 0;
            bool _datagram_starts[TX_BUFFER_SIZE_PACKETS];
        };

        PacketBuffer _buffer;
    };

    

    class Deserialiser
    {
    public:

        struct Datagram
        {
            Node src;
            Node dst;
            Buffer<MAX_PAYLOAD_SIZE> payload;
        };

        uint32_t getAvailableBufferSpace()
        {
            return MAX_SIZE_TOTAL - _buffer.size;
        };

        std::optional<Datagram> feed(uint8_t* data, uint32_t size)
        {
            // Check we are not fed too much data, this should not happen if used properly
            if(size > getAvailableBufferSpace()) { return std::nullopt; }
            
            // Copy the incoming data in our buffer
            std::memcpy(_buffer.data.data() + _buffer.size, data, size);

            // Find the next MAGIC "DOS" and truncate all the data before it
            bool magic_found = false;
            for(uint32_t i = 0; i < (_buffer.size - 2); i++)
            {
                if(_buffer.data[i] == 'D' && _buffer.data[i+1] == 'P')
                {
                    magic_found = true;
                    truncateBufferStart(i);
                    break;
                }
            }
            if(!magic_found) { return std::nullopt; }

            DefragmentationData defragmentation_data;
            while(true)
            {
                switch (parseNextPacket(defragmentation_data))
                {
                    case ParseResult::COMPLETE:
                    {
                        // Truncate the beginning of the buffer and return the datagram
                        truncateBufferStart(defragmentation_data.total_size);
                        return defragmentation_data.datagram;
                    }
                    case ParseResult::PARSE_NEXT:
                    {
                        continue;
                    }
                    case ParseResult::NOT_ENOUGH_DATA:
                    {
                        // Keep the data in the buffer and wait for more
                        return std::nullopt;
                    }
                    case ParseResult::ERROR:
                    {
                        // Truncate the beginning of the buffer and try again later
                        truncateBufferStart(defragmentation_data.total_size);
                        return std::nullopt;
                    }
                }
            }
        }

    private:

        struct DefragmentationData
        {
            Datagram datagram;
            uint8_t last_sequence_index = 0;
            uint8_t sequence_index = 0;
            uint16_t crc = 0;
            uint32_t total_size = 0;
        };

        enum class ParseResult
        {
            COMPLETE,
            PARSE_NEXT,
            NOT_ENOUGH_DATA,
            ERROR,
        };

        ParseResult parseNextPacket(DefragmentationData& defragmentation_data)
        {
            if(defragmentation_data.last_sequence_index >= MAX_FRAGMENTS) { return ParseResult::ERROR; }

            if(defragmentation_data.total_size + 6 > _buffer.size)
            {
                return ParseResult::NOT_ENOUGH_DATA;
            }

            if( _buffer.data[defragmentation_data.total_size] != 'D' || 
                _buffer.data[defragmentation_data.total_size + 1] != 'P')
            {
                defragmentation_data.total_size += 2;
                return ParseResult::ERROR;
            }

            uint8_t src =                   (_buffer.data[defragmentation_data.total_size + 2] >> 0) & 0x0F;
            uint8_t dst =                   (_buffer.data[defragmentation_data.total_size + 2] >> 4) & 0x0F;
            uint8_t size =                  _buffer.data[defragmentation_data.total_size + 3];
            uint8_t last_sequence_index =    (_buffer.data[defragmentation_data.total_size + 4] >> 0) & 0x0F;
            uint8_t sequence_index =        (_buffer.data[defragmentation_data.total_size + 4] >> 4) & 0x0F;

            // Init src and dst
            if(defragmentation_data.sequence_index == 0)
            {
                defragmentation_data.last_sequence_index = last_sequence_index;
                defragmentation_data.datagram.src = (Node)src;
                defragmentation_data.datagram.dst = (Node)dst;
            }

            // Check header is valid
            if( (   src != Node::FLIGHT_CONTROLLER && 
                    src != Node::GROUND_STATION && 
                    src != Node::PAYLOAD_COMPUTER && 
                    src != Node::REMOTE_CONTROL) ||
                size > MAX_PACKET_SIZE)
            {
                defragmentation_data.total_size += 2;
                return ParseResult::ERROR;
            }

            // Check header matches the current sequence
            if( src != defragmentation_data.datagram.src ||
                dst != defragmentation_data.datagram.dst ||
                last_sequence_index != defragmentation_data.last_sequence_index ||
                sequence_index != defragmentation_data.sequence_index)
            {
                return ParseResult::ERROR;
            }

            // Check we have enough data to read the packet
            if(_buffer.size < defragmentation_data.total_size + size)
            {
                return ParseResult::NOT_ENOUGH_DATA;
            }

            // Move payload data into datagram
            std::memcpy(defragmentation_data.datagram.payload.data.data() + defragmentation_data.datagram.payload.size, 
                        _buffer.data.data() + defragmentation_data.total_size + HEADER_SIZE,
                        size - PACKET_OVERHEAD_BYTES);
            defragmentation_data.datagram.payload.size += size - PACKET_OVERHEAD_BYTES;

            // Check CRC
            uint8_t crc_0 = _buffer.data[defragmentation_data.total_size + size - 2];
            uint8_t crc_1 = _buffer.data[defragmentation_data.total_size + size - 1];
            uint8_t expected_crc_0, expected_crc_1;
            CRC_16(defragmentation_data.datagram.payload.data.data(), defragmentation_data.datagram.payload.size, expected_crc_0, expected_crc_1);
            if(crc_0 != expected_crc_0 || crc_1 != expected_crc_1)
            {
                defragmentation_data.total_size += 2;
                return ParseResult::ERROR;
            }

            if(defragmentation_data.last_sequence_index == defragmentation_data.sequence_index)
            {
                return ParseResult::COMPLETE;
            }
            else
            {
                defragmentation_data.sequence_index++;
                return ParseResult::PARSE_NEXT;
            }
            
        }

        void truncateBufferStart(uint32_t n)
        {
            std::memmove(_buffer.data.data(), _buffer.data.data() + n, _buffer.size - n);
            _buffer.size -= n;
        }

        Buffer<MAX_SIZE_TOTAL> _buffer;
    };

    
}