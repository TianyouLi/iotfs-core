#!/bin/bash
CUR_DIR=`pwd`
BLD_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd ${BLD_DIR}

STYLE=${BLD_DIR}/.clang-format
FORMAT="clang-format -i "

echo "find ${BLD_DIR}/../src ${BLD_DIR}/../inc -iname "*.cpp" -o -iname "*.h" | xargs ${FORMAT}"
find ${BLD_DIR}/../src ${BLD_DIR}/../inc -iname "*.cpp" -o -iname "*.h" | xargs ${FORMAT}



cd ${CUR_DIR}
