#include <dragonfly_msgs/enums/EventState.h>

base_types::Enum<enums::EventState> rf_link_rw_error
base_types::Enum<enums::EventState> rf_link_rx_corrupted_data
base_types::Enum<enums::EventState> rf_link_tx_data_discarded
base_types::Enum<enums::EventState> rf_link_tx_unexpectedly_big_packet

base_types::Enum<enums::EventState> spi_link_rw_error
base_types::Enum<enums::EventState> spi_link_rx_corrupted_data
base_types::Enum<enums::EventState> spi_link_tx_data_discarded
base_types::Enum<enums::EventState> spi_link_tx_unexpectedly_big_packet