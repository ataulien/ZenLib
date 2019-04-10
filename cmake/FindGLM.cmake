#[=======================================================================[.rst:
FindGLM
-------

Finds the glm library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``GLM::GLM``
  The GLM library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``GLM_FOUND``
  True if the system has the GLM library.
``GLM_VERSION``
  The version of the GLM library which was found.
``GLM_INCLUDE_DIRS``
  Include directories needed to use GLM.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``GLM_INCLUDE_DIR``
  The directory containing ``glm/glm.hpp``.

#]=======================================================================]

find_package(PkgConfig QUIET)
pkg_check_modules(PC_glm QUIET glm)

find_path(GLM_INCLUDE_DIR
  NAMES glm
  PATHS ${PC_glm_INCLUDE_DIRS}
)

set(GLM_VERSION ${PC_glm_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM
  FOUND_VAR GLM_FOUND
  REQUIRED_VARS
    GLM_INCLUDE_DIR
  VERSION_VAR GLM_VERSION
)

if(GLM_FOUND)
  set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIR})
  set(GLM_DEFINITIONS ${PC_glm_CFLAGS_OTHER})
endif()

if(GLM_FOUND AND NOT TARGET GLM::GLM)
  add_library(GLM::GLM UNKNOWN IMPORTED)
  set_target_properties(GLM::GLM PROPERTIES
    INTERFACE_COMPILE_OPTIONS "${PC_glm_INCLUDE_DIRS}"
    INTERFACE_INCLUDE_DIRECTORIES "${GLM_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  GLM_INCLUDE_DIR
)