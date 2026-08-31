#!/bin/bash

source $DRAGONFLY_SCRIPT_UTILS

build $SCRIPT_RELATIVE_PATH "Tests"

pretty_print "Running Tests..."

run_build_docker "./$SCRIPT_RELATIVE_PATH/build/dragonfly_tests"

pretty_print "Generating Code coverage..."

run_build_docker "\
    cd $SCRIPT_RELATIVE_PATH && \
    mkdir -p build/code_coverage && \
    cd .. && \
    gcovr -r . --html --html-details -o test/build/code_coverage/coverage.html --filter '^src/' --exclude '^src/common/dragonfly_msgs'"