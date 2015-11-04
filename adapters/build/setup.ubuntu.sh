#!/bin/bash

CUR_DIR=`pwd`
BLD_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

ROOT_DIR=${BLD_DIR}/..

cd ${ROOT_DIR}

APT="sudo apt-get install -y"

# install required tools for
${APT} cmake gsoap xsltproc realpath automake
