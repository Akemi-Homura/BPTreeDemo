cmake_minimum_required(VERSION 3.0)
project(BPTDemo)

set(CMAKE_CXX_STANDARD 11)

configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif ()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif ()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif ()

add_executable(OrderedLinkListTest
        OrderedLinkListTest.cpp)

set(TESTS
        OrderedLinkListTest;
        )

#set($ENV{GTEST_CATCH_EXCEPTIONS} 0)
#target_link_libraries(BPTreeTest ${GTEST_LDFLAGS})
#target_compile_options(BPTreeTest PUBLIC ${GTEST_CFLAGS})

foreach (x ${TESTS})
    target_link_libraries(${x} gtest_main)
endforeach ()

include(CTest)

add_test(LinkListTest OrderedLinkListTest)
#add_test(BPNTest BPNodeTest)
#add_test(BPTTEST BPTreeTest)
#add_test(BPTMTest BPMoreDataTest)
#add_executable(testapp samples/sample3_unittest.cc)
#target_link_libraries(testapp ${GTEST_LDFLAGS})
#target_compile_options(testapp PUBLIC ${GTEST_CFLAGS})
#
#include(CTest)
#add_test(first_and_only_test testapp)

add_executable(main
        BPlusTree.h
        main.cpp)