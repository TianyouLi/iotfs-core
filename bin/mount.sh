#!/bin/bash
CUR_DIR=`pwd`
BLD_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

export IOTFS_BIN_DIR=${BLD_DIR}

cd ${BLD_DIR}
mkdir -p temp
./iotfs -d ./temp
cd ${CUR_DIR}
