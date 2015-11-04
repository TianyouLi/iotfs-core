#!/bin/bash
CUR_DIR=`pwd`
BLD_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd ${BLD_DIR}

# download googletest from github
git clone https://github.com/TianyouLi/CppMicroServices.git

# create build dir
mkdir -p ${BLD_DIR}/build

cd ${BLD_DIR}/build
cmake -DUS_ENABLE_THREADING_SUPPORT=yes -DUS_BUILD_TESTING=yes -DCMAKE_CXX_COMPILER_ARG1=-std=c++0x ${BLD_DIR}/CppMicroServices
make
cd ${CUR_DIR}
