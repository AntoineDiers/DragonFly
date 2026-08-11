cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2)
include(pico_sdk_import.cmake)
project(remote_control)

pico_sdk_init()

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(remote_control ${SOURCES})

target_link_libraries(remote_control pico_stdlib hardware_adc hardware_i2c)
target_include_directories(remote_control PRIVATE
    ../common/dragonfly_msgs/include
    ../common/dragonfly_datagram_protocol/include
    ../common/dragonfly_utils/include
    ../common/rpi_pico_utils/include
    ../common/config/include
    )

target_compile_options(remote_control PRIVATE
    -Wall
    -Wextra
    -Werror
)

pico_enable_stdio_usb(remote_control 1)
pico_enable_stdio_uart(remote_control 0)

pico_add_extra_outputs(remote_control)