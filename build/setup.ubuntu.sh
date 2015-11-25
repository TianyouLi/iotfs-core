#!/bin/bash

CUR_DIR=`pwd`
BLD_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

ROOT_DIR=${BLD_DIR}/../../

cd ${ROOT_DIR}

sudo apt-get update

APT="sudo apt-get install -y"

# install required libraries for fusekit
${APT} fuse libfuse-dev

# install required tools and libraries for iotivity
${APT} scons libglib2.0-dev libexpat1-dev libboost-dev libboost-thread-dev zip unzip uuid-dev libssl-dev

# install required tools for iotfs
${APT} libboost-log-dev lcov realpath

# install required tools for
${APT} cmake

cd ${CUR_DIR}
