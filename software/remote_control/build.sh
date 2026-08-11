#!/bin/bash

set -e

SCRIPT_PATH=$(dirname $(realpath $0))

export PICO_SDK_PATH=$SCRIPT_PATH/../deps/pico-sdk

cd $SCRIPT_PATH
rm -rf build
mkdir build
cd build
cmake ..
make