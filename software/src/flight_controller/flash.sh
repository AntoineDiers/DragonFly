#!/bin/bash

set -e

SCRIPT_PATH=$(dirname $(realpath $0))

cp $SCRIPT_PATH/build/flight_controller.uf2 /media/$USER/RP2350/