CMAKE_MINIMUM_REQUIRED ( VERSION 2.6)

PROJECT( onvif_oic_server )

## set output directory
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin )

file ( GLOB_RECURSE onvif_oic_server_SRC
	"${CMAKE_SOURCE_DIR}/../src/onvif_oic_server.cpp"
	"${CMAKE_SOURCE_DIR}/../src/gen-cpp/OnvifAdapter.cpp"
	"${CMAKE_SOURCE_DIR}/../src/gen-cpp/service_*.cpp"
	)
file ( GLOB_RECURSE onvif_oic_client_SRC "${CMAKE_SOURCE_DIR}/../src/onvif_oic_client.cpp" )
	
ADD_EXECUTABLE ( onvif_oic_server  ${onvif_oic_server_SRC} )
ADD_EXECUTABLE ( onvif_oic_client  ${onvif_oic_client_SRC} )

INCLUDE_DIRECTORIES (
	${CMAKE_SOURCE_DIR}/../deps/boost-process
	${CMAKE_SOURCE_DIR}/../../../../iotivity/resource/include
	${CMAKE_SOURCE_DIR}/../../../../iotivity/resource/c_common
	${CMAKE_SOURCE_DIR}/../../../../iotivity/resource/csdk/stack/include
	${CMAKE_SOURCE_DIR}/../../../../iotivity/resource/oc_logger/include
	)

FIND_LIBRARY (OC oc)
if (NOT OC)
  SET ( IOTIVITY_LIB_PATH ${CMAKE_SOURCE_DIR}/../../../../iotivity/out/linux/x86_64/release )
  SET ( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${IOTIVITY_LIB_PATH} )
  LINK_DIRECTORIES ( ${IOTIVITY_LIB_PATH} )
  FIND_LIBRARY (OC oc ${IOTIVITY_LIB_PATH} )
endif ()

TARGET_LINK_LIBRARIES ( onvif_oic_server
	${OC}
	octbstack
	oc_logger
	connectivity_abstraction
	coap
	glib-2.0
	rt
	dl
	m
	boost_system 
	boost_thread
	pthread
	thrift
	)

TARGET_LINK_LIBRARIES ( onvif_oic_client
	${OC}
	octbstack
	oc_logger
	connectivity_abstraction
	coap
	glib-2.0
	rt
	dl
	m
	pthread
	)

SET ( GCC_COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=27 -Wall -std=c++0x -g")
SET ( GCC_LINK_FLAGS    "-Wl,-rpath,$(IOTIVITY_LIB_PATH) -g")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}" )