cmake_minimum_required(VERSION 3.2)

project(ktsm-build)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(KTSM_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(KTSM_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(KTSM_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

#set(KTSM_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/division)

include_directories(${KTSM_INSTALL_INCLUDE_DIR})
#include_directories(${KTSM_HEADERS_DIR})

add_subdirectory(src)
add_subdirectory(test)


