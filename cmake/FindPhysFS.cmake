#[=======================================================================[.rst:
FindPhysFS
-------

Finds the PhysFS library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``PhysFS::PhysFS``
  The PhysFS library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``PhysFS_FOUND``
  True if the system has the PhysFS library.
``PhysFS_VERSION``
  The version of the PhysFS library which was found.
``PhysFS_INCLUDE_DIRS``
  Include directories needed to use PhysFS.
``PhysFS_LIBRARIES``
  Libraries needed to link to PhysFS.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``PhysFS_INCLUDE_DIR``
  The directory containing ``physfs.h``.
``PhysFS_LIBRARY``
  The path to the PhysFS library.

#]=======================================================================]

find_path(PhysFS_INCLUDE_DIR
  NAMES physfs.h
)

find_library(PhysFS_LIBRARY
  NAMES physfs libphysfs
)

file(READ "${PhysFS_INCLUDE_DIR}/physfs.h" _PHYSFS_HEADER)

if(_PHYSFS_HEADER MATCHES "#define[ \t]+PHYSFS_VER_MAJOR[ \t]+([0123456789]+)")
  set(PhysFS_VERSION ${CMAKE_MATCH_1})

  if(_PHYSFS_HEADER MATCHES "#define[ \t]+PHYSFS_VER_MINOR[ \t]+([0123456789]+)")
    set(PhysFS_VERSION "${PhysFS_VERSION}.${CMAKE_MATCH_1}")

    if(_PHYSFS_HEADER MATCHES "#define[ \t]+PHYSFS_VER_PATCH[ \t]+([0123456789]+)")
      set(PhysFS_VERSION "${PhysFS_VERSION}.${CMAKE_MATCH_1}")
    endif()
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PhysFS
  FOUND_VAR PhysFS_FOUND
  REQUIRED_VARS
    PhysFS_LIBRARY
    PhysFS_INCLUDE_DIR
  VERSION_VAR PhysFS_VERSION
)

if(PhysFS_FOUND)
  set(PhysFS_LIBRARIES ${PhysFS_LIBRARY})
  set(PhysFS_INCLUDE_DIRS ${PhysFS_INCLUDE_DIR})
endif()

if(PhysFS_FOUND AND NOT TARGET PhysFS::PhysFS)
  add_library(PhysFS::PhysFS UNKNOWN IMPORTED)
  set_target_properties(PhysFS::PhysFS PROPERTIES
    IMPORTED_LOCATION "${PhysFS_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${PhysFS_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  PhysFS_INCLUDE_DIR
  PhysFS_LIBRARY
  _PHYSFS_HEADER
)