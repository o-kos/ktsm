cmake_minimum_required(VERSION 3.2)
project(ktsm C CXX)

set(SOURCE_FILES
    qglobal.h
    qsharedmemory.h qsharedmemory_p.h qsharedmemory.cpp qsharedmemory_win.cpp
    qsystemsemaphore.h qsystemsemaphore_p.h qsystemsemaphore.cpp qsystemsemaphore_win.cpp
    sha1.hpp
)

add_library(ktsm SHARED STATIC ${SOURCE_FILES})

install(TARGETS ktsm DESTINATION ${KTSM_INSTALL_LIB_DIR})
install(FILES qglobal.h DESTINATION ${KTSM_INSTALL_INCLUDE_DIR})
install(FILES qsharedmemory.h DESTINATION ${KTSM_INSTALL_INCLUDE_DIR})
install(FILES qsharedmemory_p.h DESTINATION ${KTSM_INSTALL_INCLUDE_DIR})
install(FILES qsystemsemaphore.h DESTINATION ${KTSM_INSTALL_INCLUDE_DIR})
install(FILES qsystemsemaphore_p.h DESTINATION ${KTSM_INSTALL_INCLUDE_DIR})
