cmake_minimum_required(VERSION 3.5)

project(search_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(nlohmann_json)

add_executable(
    search_engine

    converterjson.h
    converterjson.cpp

    invertedindex.h
    invertedindex.cpp

    searchserver.h
    searchserver.cpp

    main.cpp
)

add_executable(
    unit_test

    converterjson.h
    converterjson.cpp

    invertedindex.h
    invertedindex.cpp

    searchserver.h
    searchserver.cpp

    tests.cpp
)
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(unit_test PRIVATE nlohmann_json::nlohmann_json)

include(FetchContent)

FetchContent_Declare(
        googletest
        URL
        https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

##set(gtest_disable_pthreads on)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

target_link_libraries(unit_test PRIVATE gtest_main)

include(GoogleTest)

gtest_discover_tests(unit_test)
