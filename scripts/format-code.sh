#!/bin/bash
CUR_DIR=`pwd`
BLD_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd ${BLD_DIR}

STYLE=${BLD_DIR}/.clang-format
FORMAT="clang-format -i "

FILES=`find ${BLD_DIR}/../src ${BLD_DIR}/../inc -iname "*.cpp" -o -iname "*.h"`
LICENSE=${BLD_DIR}/../LICENSE

for f in ${FILES}
do
	if ! grep -q Copyright ${f}
	then
		echo "Add license header to ${f}"
		(echo "/*"; cat ${LICENSE}; echo "*/"; echo; cat ${f}) >${f}.new && mv ${f}.new ${f}
	fi

	echo "Formatting code for ${f}"
	${FORMAT} ${f}
done


cd ${CUR_DIR}
