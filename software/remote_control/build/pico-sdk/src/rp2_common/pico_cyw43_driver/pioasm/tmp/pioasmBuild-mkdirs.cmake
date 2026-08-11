# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/adi/ws/DragonFly/software/deps/pico-sdk/tools/pioasm"
  "/home/adi/ws/DragonFly/software/remote_control/build/pioasm"
  "/home/adi/ws/DragonFly/software/remote_control/build/pioasm-install"
  "/home/adi/ws/DragonFly/software/remote_control/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/home/adi/ws/DragonFly/software/remote_control/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
  "/home/adi/ws/DragonFly/software/remote_control/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/home/adi/ws/DragonFly/software/remote_control/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/adi/ws/DragonFly/software/remote_control/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/adi/ws/DragonFly/software/remote_control/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
