cmake_minimum_required(VERSION 2.8.12)
project(Sonoscanner)

add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# create application
file(GLOB LIB_HEADERS src/*.h )
file(GLOB LIB_SOURCES src/*.cpp )
list(REMOVE_ITEM LIB_SOURCES src/main.cpp)
add_library(dlviewer ${LIB_SOURCES} ${LIB_HEADERS})
target_link_libraries(dlviewer ${CONAN_LIBS})
add_executable(viewer src/main.cpp)
target_link_libraries(viewer dlviewer)

# create tests
enable_testing()
file(GLOB TEST_SOURCES test/*.cpp)
file(GLOB TEST_HEADERS test/*.h)
add_executable(test_viewer ${TEST_SOURCES} ${TEST_HEADERS})
target_link_libraries(test_viewer gtest gtest_main dlviewer)
include(GoogleTest)
gtest_discover_tests(test_viewer)
