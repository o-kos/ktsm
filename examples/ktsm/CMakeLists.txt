cmake_minimum_required(VERSION 3.2)
project(ktsmex)

set(SOURCE_FILES main.cpp ../main.hpp)

add_executable(ktsmex ${SOURCE_FILES})
target_link_libraries(ktsmex ktsm)

set_target_properties(ktsmex PROPERTIES OUTPUT_NAME "ktsm")

install(TARGETS ktsmex DESTINATION ${KTSM_INSTALL_BIN_DIR})
