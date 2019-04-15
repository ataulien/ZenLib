#[=======================================================================[.rst:
FindLibSquish
-------

Finds the libsquish library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``Squish::squish``
  The libsquish library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``LibSquish_FOUND``
  True if the system has the libsquish library.
``LibSquish_VERSION``
  The version of the libsquish library which was found.
``LibSquish_INCLUDE_DIRS``
  Include directories needed to use libsquish.
``LibSquish_LIBRARIES``
  Libraries needed to link to libsquish.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``LibSquish_INCLUDE_DIR``
  The directory containing ``squish.h``.
``LibSquish_LIBRARY``
  The path to the libsquish library.

#]=======================================================================]

find_package(PkgConfig QUIET)
pkg_check_modules(PC_libsquish QUIET libsquish)

find_path(LibSquish_INCLUDE_DIR
  NAMES squish.h
  PATHS ${PC_libsquish_INCLUDE_DIRS}
  PATH_SUFFIXES libsquish
)

find_library(LibSquish_LIBRARY
  NAMES squish libsquish squishd libsquishd
  PATHS ${PC_libsquish_LIBRARY_DIRS}
)

set(LibSquish_VERSION ${PC_libsquish_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibSquish
  FOUND_VAR LibSquish_FOUND
  REQUIRED_VARS
    LibSquish_LIBRARY
    LibSquish_INCLUDE_DIR
  VERSION_VAR LibSquish_VERSION
)

if(LibSquish_FOUND)
  set(LibSquish_LIBRARIES ${LibSquish_LIBRARY})
  set(LibSquish_INCLUDE_DIRS ${LibSquish_INCLUDE_DIR})
  set(LibSquish_DEFINITIONS ${PC_libsquish_CFLAGS_OTHER})
endif()

if(LibSquish_FOUND AND NOT TARGET Squish::squish)
  add_library(Squish::squish UNKNOWN IMPORTED)
  set_target_properties(Squish::squish PROPERTIES
    IMPORTED_LOCATION "${LibSquish_LIBRARY}"
    INTERFACE_COMPILE_OPTIONS "${PC_libsquish_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${LibSquish_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  LibSquish_INCLUDE_DIR
  LibSquish_LIBRARY
)