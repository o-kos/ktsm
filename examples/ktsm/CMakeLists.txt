cmake_minimum_required(VERSION 3.2)
project(ktsmex)

set(SOURCE_FILES main.cpp)

add_executable(ktsmex ${SOURCE_FILES})
target_link_libraries(ktsmex ktsm)
install(TARGETS ktsmex DESTINATION ${DIVISIBLE_INSTALL_BIN_DIR})
