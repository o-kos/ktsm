cmake_minimum_required(VERSION 3.24)
project(qtsm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin")

set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/mingw81_64")

find_package(Qt5 COMPONENTS
        Core
        REQUIRED)

add_executable(qtsm main.cpp ../main.hpp)
target_link_libraries(qtsm Qt5::Core)

install(TARGETS qtsm DESTINATION ${KTSM_INSTALL_BIN_DIR})
