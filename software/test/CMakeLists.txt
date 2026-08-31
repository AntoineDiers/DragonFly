cmake_minimum_required(VERSION 3.13)
project(dragonfly_tests)

add_subdirectory(../deps/googletest googletest)

file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp")
list(FILTER ALL_SOURCES EXCLUDE REGEX "/main.cpp")
list(FILTER ALL_SOURCES EXCLUDE REGEX "/build/")

add_executable(dragonfly_tests ${ALL_SOURCES} src/main.cpp)

target_link_libraries(dragonfly_tests PRIVATE GTest::gtest_main)
target_include_directories(dragonfly_tests PRIVATE
    ../src/common/config/include
    ../src/common/dragonfly_msgs/include
    ../src/common/ads1115_driver/include
    ../src/common/dragonfly_datagram_protocol/include
    ../src/common/dragonfly_utils/include
    ../src/common/rpi_pico_utils/include)
target_compile_options(dragonfly_tests PRIVATE --coverage -g)
target_link_options(dragonfly_tests PRIVATE --coverage)