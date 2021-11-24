cmake_minimum_required(VERSION 2.8.12)
project(Sonoscanner)

add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB   LIB_SOURCES src/*.cpp )
list(REMOVE_ITEM LIB_SOURCES src/main.cpp)
file(GLOB LIB_HEADERS src/*.h )
add_executable(viewer src/main.cpp ${LIB_SOURCES} ${LIB_HEADERS})
target_link_libraries(viewer ${CONAN_LIBS})
