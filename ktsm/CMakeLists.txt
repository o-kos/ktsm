cmake_minimum_required(VERSION 3.24)
project(qtsm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin")

add_executable(ktsm main.cpp ../main.hpp
    qsharedmemory.cpp qsharedmemory.h qsharedmemory_p.h qsharedmemory_win.cpp
    qsystemsemaphore.cpp qsystemsemaphore.h qsystemsemaphore_p.h qsystemsemaphore_win.cpp)
