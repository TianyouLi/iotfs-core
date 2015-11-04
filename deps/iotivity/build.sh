#!/bin/bash
CUR_DIR=`pwd`
BLD_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd ${BLD_DIR}

# download googletest from github
git clone https://github.com/TianyouLi/iotivity.git

# create build dir
cd ${BLD_DIR}/iotivity
git checkout 1.0.0-dev
git clone https://github.com/01org/tinycbor.git extlibs/tinycbor/tinycbor
scons

cd ${CUR_DIR}
