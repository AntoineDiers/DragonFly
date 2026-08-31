#!/bin/bash

set -e

SCRIPT_ABSOLUTE_PATH="$(dirname "$(realpath "${BASH_SOURCE[1]}")")"
SCRIPT_RELATIVE_PATH="$(realpath --relative-to=$DRAGONFLY_ROOT_PATH "$(dirname "$(realpath "${BASH_SOURCE[1]}")")")"

pretty_print() {
    local message="$1"
    local min_width=70
    local color='\033[96m'
    local reset='\033[0m'

    local message_width=${#message}
    local width=$((message_width + 2))

    if (( width < min_width )); then
        width=$min_width
    fi

    local padding=$((width - message_width))
    local left_padding=$((padding / 2))
    local right_padding=$((padding - left_padding))

    printf "\n"
    printf "${color}%*s${reset}\n" "$width" "" | tr ' ' '-'
    printf "${color}%*s%s%*s${reset}\n" \
        "$left_padding" "" \
        "$message" \
        "$right_padding" ""
    printf "${color}%*s${reset}\n" "$width" "" | tr ' ' '-'
    printf "\n"
}

generate_build_docker()
{
    pretty_print "Generating Build Docker Image..."

    cd $DRAGONFLY_ROOT_PATH/software/docker
    docker build . -t dragonfly-build -f DockerFileBuild
    cd - > /dev/null
}

run_build_docker()
{
    docker run -t --rm --user "$(id -u):$(id -g)" -v $DRAGONFLY_ROOT_PATH:/workspace $DOCKER_BUILD_IMAGE_NAME bash -c "cd /workspace && $1"
}

build()
{
    RELATIVE_PATH=$1
    BUILD_NAME=$2

    pretty_print "Building $BUILD_NAME..."

    run_build_docker "\
        cd $RELATIVE_PATH && \
        rm -rf build && \
        mkdir -p build && \
        cd build && \
        export PICO_SDK_PATH=../../../deps/pico-sdk && \
        cmake .. && \
        make"
}

if [[ -z "${BUILD_DOCKER_GENERATED:-}" ]]; then
    generate_build_docker
    export BUILD_DOCKER_GENERATED=1
fi