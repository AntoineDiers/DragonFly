#!/bin/bash

set -e

SCRIPT_PATH=$(dirname $(realpath $0))

COLOR_OFF='\033[0m'
COLOR_LIGHT_BLUE='\033[1;36m'

log()
{
    printf "\n${COLOR_LIGHT_BLUE}\n"
    printf " ------------------------------------------------------------------\n"
    printf "   $1\n"
    printf " ------------------------------------------------------------------\n"
    printf "${COLOR_OFF}\n\n"
}

# -----------------------------------------------------

log "Building sphinx docker image ..."

cd $SCRIPT_PATH/../software/docker
docker build -t dragonfly-build-x86 -f DockerfileBuild_x86 .

# -----------------------------------------------------

log "Building documentation ..."
cd $SCRIPT_PATH/..
if [[ $1 = "--dev" ]]; then
    CMD="sphinx-autobuild --port 8001 /workspace/docs_src /workspace/docs"
else
    CMD="sphinx-build /workspace/docs_src /workspace/docs"
fi
docker run -t --rm -u $(id -u ${USER}):$(id -g ${USER}) --network host -v $PWD:/workspace dragonfly-build-x86 bash -c "$CMD"