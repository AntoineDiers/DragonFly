cmake_minimum_required(VERSION 3.13)

project(dragonfly_msgs)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(dragonfly_msgs_generator ${SOURCES})