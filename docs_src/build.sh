#!/bin/bash

source $DRAGONFLY_SCRIPT_UTILS

pretty_print "Generating Documentation..."

if [[ $1 = "--dev" ]]; then
    CMD="sphinx-autobuild --port 8001 ./docs_src ./docs"
else
    CMD="sphinx-build ./docs_src ./docs"
fi

run_build_docker "$CMD"