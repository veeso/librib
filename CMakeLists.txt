cmake_minimum_required(VERSION 3.0)
project(librib VERSION 1.0.1)

execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set (ROOT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set (RIB_SOURCE_DIR "${ROOT_SOURCE_DIR}/rib/")
set (ROUTER_SOURCE_DIR "${ROOT_SOURCE_DIR}/router/")

file(GLOB RIB_SRC
  "${RIB_SOURCE_DIR}/*.c"
)

file(GLOB ROUTER_SRC
  "${ROUTER_SOURCE_DIR}/*.c"
)

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

#Includes
file(GLOB INCLUDE_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/include/rib/*.h"
)

#Build options
if (WITH_ROUTER)
  add_definitions(-DWITH_ROUTER)
endif(WITH_ROUTER)


#Check if C11 is supported
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c11 HAVE_C11)

set(CXX_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
if (HAVE_C11)
  set(CMAKE_C_STANDARD 11)
else()
  set(CMAKE_C_STANDARD 99)
endif(HAVE_C11)
set(CMAKE_BUILD_TYPE DEBUG)

add_definitions("-DRIB_GIT_COMMIT=${GIT_COMMIT}")

add_library(rib_shared SHARED ${RIB_SRC})
set_target_properties(rib_shared PROPERTIES OUTPUT_NAME rib)
set_target_properties(rib_shared PROPERTIES VERSION 1.0.1)
add_library(rib_static STATIC ${RIB_SRC})
set_target_properties(rib_static PROPERTIES OUTPUT_NAME rib)


if (WITH_ROUTER)
  add_executable(router ${ROUTER_SRC})
  target_link_libraries(router PUBLIC rib_shared)
endif(WITH_ROUTER)

#Install rules
install(TARGETS rib_shared CONFIGURATIONS Release LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include)
install(TARGETS rib_static CONFIGURATIONS Release ARCHIVE DESTINATION lib)
install(FILES ${INCLUDE_FILES} DESTINATION include/rib/)
if (WITH_ROUTER)
  install(TARGETS router CONFIGURATIONS Release RUNTIME DESTINATION bin)
endif (WITH_ROUTER)
