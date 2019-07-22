cmake_minimum_required (VERSION 3.1)
project(PNG2ICO)

set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CXX_STANDAR 17)
set(CMAKE_CXX_STANDAR_REQUIRED ON)

set(CMAKE_LIB_DIR CACHE PATH "path to lib Folder")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_compile_options( /std:c++17)
else()
	add_compile_options( -std=c++17 -Wall -pedantic -Wextra)
endif()

file(GLOB SOURCE_FILES_API "src/api.*")

add_library(png2ico ${SOURCE_FILES_API})
target_include_directories(png2ico PRIVATE "${CMAKE_LIB_DIR}/CImg-2.6.7" "${CMAKE_LIB_DIR}/libpng" "${CMAKE_LIB_DIR}/libpng/build")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_link_libraries(png2ico 
		debug ${CMAKE_LIB_DIR}/libpng/build/bin/libpng16d.lib
		optimized ${CMAKE_LIB_DIR}/libpng/build/bin/libpng16.lib)
else()
	target_link_libraries(png2ico stdc++fs  
		debug ${CMAKE_LIB_DIR}/libpng/build/bin/libpng16d.lib
		optimized ${CMAKE_LIB_DIR}/libpng/build/bin/libpng16.lib)
endif()

file(GLOB SOURCE_FILES_CMD "src/main.cpp")

add_executable(interface ${SOURCE_FILES_CMD})
target_include_directories(interface PRIVATE src)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_link_libraries(interface png2ico)
else()
	target_link_libraries( ${CMAKE_LIB_DIR} stdc++fs)
	target_link_libraries(interface png2ico)
endif()