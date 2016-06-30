cmake_minimum_required(VERSION 2.4)
project(ZenLib)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 14)

# Mute annoying MSVC-Warnings
if(MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	add_definitions(-DNOMINMAX)
endif()

# 3rd-party dependencies
add_subdirectory(lib/libsquish)

include_directories(lib/glm/glm)
include_directories(lib/libsquish)
include_directories(.)

# Internal libraries
add_subdirectory(utils)
add_subdirectory(vdfs)
add_subdirectory(zenload)
add_subdirectory(samples)
add_subdirectory(daedalus)
