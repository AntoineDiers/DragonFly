#!/bin/bash

source $DRAGONFLY_SCRIPT_UTILS

$DRAGONFLY_ROOT_PATH/docs_src/build.sh
$DRAGONFLY_ROOT_PATH/software/src/flight_controller/build.sh
$DRAGONFLY_ROOT_PATH/software/src/remote_control/build.sh
$DRAGONFLY_ROOT_PATH/software/test/run.sh

pretty_print "Deploying build results..."

cd $DRAGONFLY_ROOT_PATH
rm -rf ./deploy
rm -rf ./code_coverage
mkdir -p deploy

cp software/src/flight_controller/build/flight_controller.uf2 deploy/
cp software/src/remote_control/build/remote_control.uf2 deploy/
cp -r software/test/build/code_coverage .