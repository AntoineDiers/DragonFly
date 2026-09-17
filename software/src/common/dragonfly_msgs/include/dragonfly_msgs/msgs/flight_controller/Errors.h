
#pragma once

#include <dragonfly_msgs/internal/Common.h>

#include <dragonfly_msgs/enums/ErrorState.h>


namespace dragonfly_msgs { 
namespace msgs { 
namespace flight_controller { 


class Errors
{
public:

        base_types::Enum<enums::ErrorState> rf_link_rw_error;
        base_types::Enum<enums::ErrorState> rf_link_corrupted_data;
        base_types::Enum<enums::ErrorState> rf_link_tx_data_discarded;
        base_types::Enum<enums::ErrorState> rf_link_tx_unexpectedly_big_packet;
        base_types::Enum<enums::ErrorState> spi_link_rw_error;
        base_types::Enum<enums::ErrorState> spi_link_corrupted_data;
        base_types::Enum<enums::ErrorState> spi_link_tx_data_discarded;
        base_types::Enum<enums::ErrorState> spi_link_tx_unexpectedly_big_packet;


    static constexpr uint32_t _SerialisationSizeBits = 
        base_types::Enum<enums::ErrorState>::_SerialisationSizeBits + 
        base_types::Enum<enums::ErrorState>::_SerialisationSizeBits + 
        base_types::Enum<enums::ErrorState>::_SerialisationSizeBits + 
        base_types::Enum<enums::ErrorState>::_SerialisationSizeBits + 
        base_types::Enum<enums::ErrorState>::_SerialisationSizeBits + 
        base_types::Enum<enums::ErrorState>::_SerialisationSizeBits + 
        base_types::Enum<enums::ErrorState>::_SerialisationSizeBits + 
        base_types::Enum<enums::ErrorState>::_SerialisationSizeBits + 
        0;
    static constexpr uint32_t _Id = 1;
    static constexpr uint32_t _IdSizeBits = 4;
    static constexpr uint32_t _SizeBytes = serialisation::sizeBitsToBytes(_SerialisationSizeBits + _IdSizeBits);

    template<uint32_t BufferSize>
    void _serialise(Buffer<BufferSize>& buffer) const
    {
        static_assert(BufferSize >= _SizeBytes);

        serialisation::WBitBuffer bit_buffer(buffer.data.data());
        bit_buffer.write(_Id, _IdSizeBits);
        _serialise(bit_buffer);

        buffer.size = _SizeBytes;

        return;
    }

    template<uint32_t BufferSize>
    bool _deserialise(const Buffer<BufferSize>& buffer)
    {
        static_assert(BufferSize >= _SizeBytes);

        if(buffer.size != _SizeBytes) { return false; }
        
        serialisation::RBitBuffer bit_buffer(buffer.data.data());
        if(bit_buffer.read(_IdSizeBits) != _Id) { return false; }

        _deserialise(bit_buffer);
        return true;
    }
    
    void _serialise(serialisation::WBitBuffer& buffer) const
    {
        rf_link_rw_error._serialise(buffer);
        rf_link_corrupted_data._serialise(buffer);
        rf_link_tx_data_discarded._serialise(buffer);
        rf_link_tx_unexpectedly_big_packet._serialise(buffer);
        spi_link_rw_error._serialise(buffer);
        spi_link_corrupted_data._serialise(buffer);
        spi_link_tx_data_discarded._serialise(buffer);
        spi_link_tx_unexpectedly_big_packet._serialise(buffer);
    }

    void _deserialise(serialisation::RBitBuffer& buffer)
    {
        rf_link_rw_error._deserialise(buffer);
        rf_link_corrupted_data._deserialise(buffer);
        rf_link_tx_data_discarded._deserialise(buffer);
        rf_link_tx_unexpectedly_big_packet._deserialise(buffer);
        spi_link_rw_error._deserialise(buffer);
        spi_link_corrupted_data._deserialise(buffer);
        spi_link_tx_data_discarded._deserialise(buffer);
        spi_link_tx_unexpectedly_big_packet._deserialise(buffer);
    }

    nlohmann::json _serialiseJson() const
    {
        nlohmann::json res;
        res["rf_link_rw_error"] = rf_link_rw_error._serialiseJson();
        res["rf_link_corrupted_data"] = rf_link_corrupted_data._serialiseJson();
        res["rf_link_tx_data_discarded"] = rf_link_tx_data_discarded._serialiseJson();
        res["rf_link_tx_unexpectedly_big_packet"] = rf_link_tx_unexpectedly_big_packet._serialiseJson();
        res["spi_link_rw_error"] = spi_link_rw_error._serialiseJson();
        res["spi_link_corrupted_data"] = spi_link_corrupted_data._serialiseJson();
        res["spi_link_tx_data_discarded"] = spi_link_tx_data_discarded._serialiseJson();
        res["spi_link_tx_unexpectedly_big_packet"] = spi_link_tx_unexpectedly_big_packet._serialiseJson();
        return res;
    }

};

}}}

