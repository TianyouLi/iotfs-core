#!/bin/bash
CUR_DIR=`pwd`
BLD_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd ${BLD_DIR}

# download googletest from github
git clone https://github.com/TianyouLi/fusekit.git

cd ${CUR_DIR}
