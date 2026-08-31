#!/bin/bash

source $DRAGONFLY_SCRIPT_UTILS

cd $DRAGONFLY_ROOT_PATH

./software/test/run.sh

pretty_print "Generating Documentation..."

rm -rf docs
mkdir -p docs
cp -r software/test/build/code_coverage/ docs/ 

if [[ $1 = "--dev" ]]; then
    CMD="sphinx-autobuild --port 8001 ./docs_src ./docs"
else
    CMD="sphinx-build ./docs_src ./docs"
fi

run_build_docker "$CMD"

touch docs/.nojekyll