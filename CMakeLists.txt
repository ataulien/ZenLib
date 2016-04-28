cmake_minimum_required(VERSION 3.3)
project(ZenLib)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 14)

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
