cmake_minimum_required(VERSION 3.0)
project(BPTDemo)

set(CMAKE_CXX_STANDARD 11)

cmake_policy(SET CMP0048 NEW)

find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest_main)

set(BPNode
        BPNode.h
        )

set(BPTree
        BPTree.h
        BPTHelper.h
        )

add_executable(BPNodeTest
        BPNodeTest.cpp
        )

add_executable(BPTreeTest
        ${BPNode}
        ${BPTree}
        BPTreeTest.cpp)

add_executable(BPMoreDataTest
        ${BPNode}
        ${BPTree}
        BPMoreDataTest.cpp)

set(TESTS
        BPNodeTest;
        BPTreeTest;
        BPMoreDataTest)

set($ENV{GTEST_CATCH_EXCEPTIONS} 0)
#target_link_libraries(BPTreeTest ${GTEST_LDFLAGS})
#target_compile_options(BPTreeTest PUBLIC ${GTEST_CFLAGS})

foreach (x ${TESTS})
    target_link_libraries(${x} ${GTEST_LDFLAGS})
    target_compile_options(${x} PUBLIC ${GTEST_CFLAGS})
endforeach ()

include(CTest)

add_test(BPNTest BPNodeTest)
add_test(BPTTEST BPTreeTest)
add_test(BPTMTest BPMoreDataTest)
#add_executable(testapp samples/sample3_unittest.cc)
#target_link_libraries(testapp ${GTEST_LDFLAGS})
#target_compile_options(testapp PUBLIC ${GTEST_CFLAGS})
#
#include(CTest)
#add_test(first_and_only_test testapp)

add_executable(BPTDemo
        main.cpp)