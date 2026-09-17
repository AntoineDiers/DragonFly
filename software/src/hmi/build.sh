#!/bin/bash

source $DRAGONFLY_SCRIPT_UTILS
build $SCRIPT_RELATIVE_PATH/backend "HMI Backend"

pretty_print "Building HMI Frontend..."

run_build_docker "\
    cd $SCRIPT_RELATIVE_PATH/frontend && \
    rm -rf dist && \
    npm run build"
    