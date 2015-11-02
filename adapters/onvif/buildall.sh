#!/bin/bash

CUR_DIR=`pwd`
BLD_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

## build thrift libraries
cd ${BLD_DIR}
cd ./deps/thrift
./configure --with-cpp=yes --with-c-glib=no --with-python=no --with-nodejs=yes
make
sudo make install

## generate thrift cpp client
cd ${BLD_DIR}
cd ./src
thrift -r --gen cpp ../deps/service.thrift

## generate thrift nodejs server
cd ${BLD_DIR}
cd ./deps/nodenvc
thrift -r --gen js:node ../service.thrift

## build binaries
cd ${BLD_DIR}
./build/build.sh

cd ${CUR_DIR}
