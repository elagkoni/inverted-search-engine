cmake_minimum_required(VERSION 3.5)

project(sigismund LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pthread")

set(SOURCE_FILES
    src/main.cpp
    src/list.h
    src/list_impl.h
    src/hashtable.h
    src/hashtable_impl.h
    src/vector.h
    src/vector_impl.h
    src/entry.h
    src/entry.cpp
    src/utilities.h
    src/utilities.cpp
    src/bktree.h
    src/bktree.cpp
    src/sigismund.h
    src/sigismund.cpp

    src/job.h
    src/job.cpp
    src/scheduler.h
    src/scheduler.cpp

    include/core.h
    )

add_executable(sigismund
    ${SOURCE_FILES}

    tests/catch.hpp
    tests/test_list.cpp
    tests/test_hashtable.cpp
    tests/test_bktree.cpp
    tests/test_utilities.cpp
    tests/test_sigismund.cpp

    test_driver/test.cpp
    test_data/small_test.txt
    test_data/tiny_test.txt

    README.md    
    )

add_definitions(-DTESTS)
add_definitions(-DMULTITHREADING)
add_definitions(-DWORD_THREADING)

include (CTest)
include (Dart)
enable_testing()

add_test(test_components sigismund)

#add_executable(list_test
#    ${SOURCE_FILES}

#    tests/catch.hpp
#    tests/test_list.cpp
#    )
#add_test(test_list list_test)

#add_executable(hashtable_test
#    ${SOURCE_FILES}

#    tests/catch.hpp
#    tests/test_hashtable.cpp
#    )
#add_test(test_hashtable hashtable_test)

#add_executable(utilities_test
#    ${SOURCE_FILES}

#    tests/catch.hpp
#    tests/test_utilities.cpp
#    )
#add_test(test_utilities utilities_test)

#add_executable(bktree_test
#    ${SOURCE_FILES}

#    tests/catch.hpp
#    tests/test_bktree.cpp
#    )
#add_test(test_bktree bktree_test)


