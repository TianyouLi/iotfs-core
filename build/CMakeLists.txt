CMAKE_MINIMUM_REQUIRED ( VERSION 2.6)

PROJECT( iotfs )

## set output directory
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin )

file ( GLOB iotfs_SRC "${CMAKE_SOURCE_DIR}/../src/*.cpp" )

ADD_EXECUTABLE ( iotfs  ${iotfs_SRC} )

# set the path for libraries
set(CppMicroServices_DIR ${CMAKE_SOURCE_DIR}/../deps/cppus/build)
find_package(CppMicroServices NO_MODULE REQUIRED)
find_package(Boost)

INCLUDE_DIRECTORIES (
	${CMAKE_SOURCE_DIR}/../inc
	${CMAKE_SOURCE_DIR}/../deps/fusekit/fusekit/include
	${CppMicroServices_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	/usr/include/fuse
	)

TARGET_LINK_LIBRARIES ( iotfs
	${CppMicroServices_LIBRARIES}
	boost_system
	boost_filesystem
	boost_log
	pthread
	fuse
	rt
	dl
	m
	)

SET ( GCC_COMPILE_FLAGS "-DIOTINFOPROVIDER_EXPORTS -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=27 -DBOOST_LOG_DYN_LINK -Wall -std=c++0x -g")
SET ( GCC_LINK_FLAGS    "-Wl,-rpath,$(IOTIVITY_LIB_PATH) -g")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}" )