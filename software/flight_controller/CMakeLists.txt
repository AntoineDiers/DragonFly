cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2_w)
include(pico_sdk_import.cmake)
project(flight_controller)

pico_sdk_init()

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(flight_controller ${SOURCES})

target_link_libraries(flight_controller pico_stdlib hardware_pwm hardware_i2c)
target_include_directories(flight_controller PRIVATE
    ../common/config/include
    ../common/dragonfly_msgs/include
    ../common/dragonfly_datagram_protocol/include
    ../common/dragonfly_utils/include
    ../common/rpi_pico_utils/include)

target_compile_options(flight_controller PRIVATE
    -Wall
    -Wextra
    -Werror
)

pico_enable_stdio_usb(flight_controller 1)
pico_enable_stdio_uart(flight_controller 0)

pico_add_extra_outputs(flight_controller)