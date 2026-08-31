#!/bin/bash

source $DRAGONFLY_SCRIPT_UTILS

$DRAGONFLY_ROOT_PATH/software/src/flight_controller/build.sh
$DRAGONFLY_ROOT_PATH/software/src/remote_control/build.sh
$DRAGONFLY_ROOT_PATH/docs_src/build.sh

pretty_print "Deploying build results..."

cd $DRAGONFLY_ROOT_PATH/software
rm -rf ./deploy
mkdir -p deploy
cp src/flight_controller/build/flight_controller.uf2 deploy/
cp src/remote_control/build/remote_control.uf2 deploy/