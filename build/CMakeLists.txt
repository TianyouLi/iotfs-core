CMAKE_MINIMUM_REQUIRED ( VERSION 2.6)

PROJECT( iotfs )

## set output directory
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin )

file ( GLOB_RECURSE iotfs_SRC "${CMAKE_SOURCE_DIR}/../src/*.cpp" )

ADD_EXECUTABLE ( iotfs  ${iotfs_SRC} )

INCLUDE_DIRECTORIES (
	${CMAKE_SOURCE_DIR}/../inc
	${CMAKE_SOURCE_DIR}/../deps/fusekit/fusekit/include
	${CMAKE_SOURCE_DIR}/../deps/iotivity/iotivity/resource/include
	${CMAKE_SOURCE_DIR}/../deps/iotivity/iotivity/resource/c_common
	${CMAKE_SOURCE_DIR}/../deps/iotivity/iotivity/resource/csdk/stack/include
	${CMAKE_SOURCE_DIR}/../deps/iotivity/iotivity/resource/oc_logger/include
	/usr/include/fuse
	)

FIND_LIBRARY (OC oc)
if (NOT OC)
  SET ( IOTIVITY_LIB_PATH ${CMAKE_SOURCE_DIR}/../deps/iotivity/iotivity/out/linux/x86_64/release )
  SET ( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${IOTIVITY_LIB_PATH} )
  LINK_DIRECTORIES ( ${IOTIVITY_LIB_PATH} )
  FIND_LIBRARY (OC oc ${IOTIVITY_LIB_PATH} )
endif ()

TARGET_LINK_LIBRARIES ( iotfs
	${OC}
	octbstack
	oc_logger
	connectivity_abstraction
	coap
	glib-2.0
	fuse
	rt
	dl
	m
	)

SET ( GCC_COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=27 -Wall -std=c++0x -g")
SET ( GCC_LINK_FLAGS    "-Wl,-rpath,$(IOTIVITY_LIB_PATH) -g")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}" )