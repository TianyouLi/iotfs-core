project(IoTFS_OIC_Plugin)
cmake_minimum_required(VERSION 2.8)

# set output library path
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../../lib )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../../lib )

# set the path for libraries
set(CppMicroServices_DIR ${CMAKE_SOURCE_DIR}/../../../deps/cppus/build)

find_package(CppMicroServices NO_MODULE REQUIRED)
include_directories(${CppMicroServices_INCLUDE_DIRS})

# set the plugin library name
set(lib_name iotfs-oic)


# set the include directory
INCLUDE_DIRECTORIES (
	${CMAKE_SOURCE_DIR}/../../../inc
	${CMAKE_SOURCE_DIR}/../../../src/oic
	${CMAKE_SOURCE_DIR}/../../../deps/fusekit/fusekit/include
	${CMAKE_SOURCE_DIR}/../../../deps/iotivity/iotivity/resource/include
	${CMAKE_SOURCE_DIR}/../../../deps/iotivity/iotivity/resource/c_common
	${CMAKE_SOURCE_DIR}/../../../deps/iotivity/iotivity/resource/csdk/stack/include
	${CMAKE_SOURCE_DIR}/../../../deps/iotivity/iotivity/resource/oc_logger/include
	/usr/include/fuse
	)

# set the plugin source file list
file( GLOB_RECURSE iotfs_oic_SRC "${CMAKE_SOURCE_DIR}/../../../src/oic/*.cpp" )


# Generate module initialization code
usFunctionGenerateModuleinit(iotfs_oic_SRC
	NAME "IoTFS OIC Plugin"
	LIBRARY_NAME ${lib_name}
	)

# set compilation flags
SET ( GCC_COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=27 -Wall -std=c++11 -g")
SET ( GCC_LINK_FLAGS    "-Wl,-rpath,$(IOTIVITY_LIB_PATH) -g")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}" )

# create the library
add_library(plugin-${lib_name} SHARED ${iotfs_oic_SRC})

# get the iotivity library
FIND_LIBRARY (OC oc)
if (NOT OC)
  SET ( IOTIVITY_LIB_PATH ${CMAKE_SOURCE_DIR}/../../../deps/iotivity/iotivity/out/linux/x86_64/release )
  SET ( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${IOTIVITY_LIB_PATH} )
  LINK_DIRECTORIES ( ${IOTIVITY_LIB_PATH} )
  FIND_LIBRARY (OC oc ${IOTIVITY_LIB_PATH} )
endif ()

# Link the required library
target_link_libraries(plugin-${lib_name}
	${CppMicroServices_LIBRARIES}
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