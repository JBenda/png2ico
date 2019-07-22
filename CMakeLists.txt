cmake_minimum_required (VERSION 3.1)
project(PNG2ICO)

set(CMAKE_CXX_STANDAR 17)
set(CMAKE_CXX_STANDAR_REQUIRED ON)

set(CMAKE_LIB_DIR "C:/Users/lukas/Documents/Programming/Libs" CACHE PATH "path to lib Folder")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_compile_options( /std:c++17)
else()
	add_compile_options( -std=c++17 -Wall -pedantic -Wextra)
endif()

file(GLOB SOURCE_FILES_API "src/api.*" "${CMAKE_LIB_DIR}/CImg.h")

add_library(png2ico ${SOURCE_FILES_API})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
else()
	target_link_libraries(png2ico ${CMAKE_LIB_DIR} stdc++fs)
endif()

file(GLOB SOURCE_FILES_CMD "main.cpp")

add_executable(interface ${SOURCE_FILES_CMD})
target_include_directories(interface PRIVATE src)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_link_libraries(interface png2ico)
else()
	target_link_libraries( ${CMAKE_LIB_DIR} stdc++fs)
	target_link_libraries(interface png2ico)
endif()