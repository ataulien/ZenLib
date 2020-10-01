cmake_minimum_required(VERSION 3.9)
project(ZenLib)

include(FetchContent)
include(cmake/clang-format.cmake)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(ZENLIB_BUILD_EXAMPLES ON CACHE BOOL "Whether to build the examples")
set(ZENLIB_BUILD_TESTS OFF CACHE BOOL "Whether to build tests")

set(CMAKE_CXX_STANDARD 14)

# -----------------------------------------------------------------------------
# External: libsquish
#------------------------------------------------------------------------------
FetchContent_Declare(
  libsquish
  URL       https://downloads.sourceforge.net/project/libsquish/libsquish-1.15.tgz
  URL_HASH  SHA1=51844b9a8bc815a27e2cc0ffbede5fee3ef75110
)

if(NOT libsquish_POPULATED)
  message(STATUS "ZenLib: Populating libsquish...")

  FetchContent_Populate(libsquish)

  SET(BUILD_SQUISH_WITH_OPENMP OFF CACHE BOOL "" FORCE)
  add_subdirectory(${libsquish_SOURCE_DIR} ${libsquish_BINARY_DIR})

  message(STATUS "ZenLib: Done populating libsquish!")
endif()

# -----------------------------------------------------------------------------
# External: physfs
#------------------------------------------------------------------------------

FetchContent_Declare(
  physfs
  URL       https://icculus.org/physfs/downloads/physfs-3.0.2.tar.bz2
  URL_HASH  SHA1=6a15c458898d0570101d8f173201cde9ced78df7
)

if(NOT physfs_POPULATED)
  message(STATUS "ZenLib: Populating physfs...")

  FetchContent_Populate(physfs)

  set(PHYSFS_BUILD_SHARED OFF)
  set(PHYSFS_BUILD_TEST OFF)
  add_subdirectory(${physfs_SOURCE_DIR} ${physfs_BINARY_DIR})

  message(STATUS "ZenLib: Done populating physfs!")
endif()

# -----------------------------------------------------------------------------

# Mute annoying MSVC-Warnings
if(MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	add_definitions(-DNOMINMAX)
endif()

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
