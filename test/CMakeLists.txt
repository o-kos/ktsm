# Tests need to be added as executables first
add_executable(testktsm testktsm.cpp catch2/catch_amalgamated.cpp)

#target_include_directories(testdmd PRIVATE "../include" "../src/core")

target_link_libraries(testktsm PRIVATE ktsm)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME testlibktsm COMMAND testktsm) # Command can be a target

