cmake_minimum_required(VERSION 3.10)
project(ZenLib)

include(ExternalProject)
include(cmake/clang-format.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(ZENLIB_BUILD_EXAMPLES OFF CACHE BOOL "Whether to build the examples")
set(ZENLIB_BUILD_TESTS OFF CACHE BOOL "Whether to build tests")

set(CMAKE_CXX_STANDARD 14)

# Mute annoying MSVC-Warnings
if(MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	add_definitions(-DNOMINMAX)
endif()

find_package(GLM REQUIRED)

include_directories(${GLM_INCLUDE_DIRS})
include_directories(.)

# Internal libraries
add_subdirectory(utils)
add_subdirectory(vdfs)
add_subdirectory(zenload)
add_subdirectory(daedalus)

# Tests
if(ZENLIB_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(ZENLIB_BUILD_EXAMPLES)
  add_subdirectory(samples)
endif()
