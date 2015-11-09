#!/bin/bash
CUR_DIR=`pwd`
BLD_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd ${BLD_DIR}
cmake -G"Eclipse CDT4 - Unix Makefiles" -D_ECLIPSE_VERSION=4.3 -DCMAKE_CXX_COMPILER_ARG1=-std=c++0x ${BLD_DIR}
make
cd ${CUR_DIR}


OIC_DIR=${BLD_DIR}/plugins/oic
cd ${OIC_DIR}
cmake -G"Eclipse CDT4 - Unix Makefiles" -D_ECLIPSE_VERSION=4.3 -DCMAKE_CXX_COMPILER_ARG1=-std=c++0x ${OIC_DIR}
make
cd ${CUR_DIR}
