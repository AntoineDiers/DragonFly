cmake_minimum_required(VERSION 3.13)

project(dragonfly_server)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(dragonfly_server ${SOURCES})

target_include_directories(dragonfly_server PRIVATE
    ../../../deps
    ../../../deps/json/include
    ../../common/dragonfly_msgs/include
    ../../common/dragonfly_utils/include)

target_compile_options(dragonfly_server PRIVATE
    -Wall
    -Wextra
    -Werror
)