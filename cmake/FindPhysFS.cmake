find_path(PhysFS_INCLUDE_DIR NAMES physfs.h)
find_library(PhysFS_LIBRARY NAMES physfs libphysfs)
mark_as_advanced(PhysFS_INCLUDE_DIR PhysFS_LIBRARY)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(PhysFS REQUIRED_VARS PhysFS_LIBRARY PhysFS_INCLUDE_DIR)

if(PhysFS_FOUND)
  if(NOT TARGET PhysFS::PhysFS)
    add_library(PhysFS::PhysFS UNKNOWN IMPORTED)
    set_target_properties(PhysFS::PhysFS PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${PhysFS_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${PhysFS_INCLUDE_DIR}")
  endif()

  if (PhysFS_INCLUDE_DIR AND EXISTS "${PhysFS_INCLUDE_DIR}/physfs.h")
    file(STRINGS "${PhysFS_INCLUDE_DIR}/physfs.h" physfs_major_version REGEX "^#define[ \\t]+PHYSFS_VER_MAJOR[ \\t]+\\d+")
    file(STRINGS "${PhysFS_INCLUDE_DIR}/physfs.h" physfs_minor_version REGEX "^#define[ \\t]+PHYSFS_VER_MINOR[ \\t]+\\d+")
    file(STRINGS "${PhysFS_INCLUDE_DIR}/physfs.h" physfs_patch_version REGEX "^#define[ \\t]+PHYSFS_VER_PATCH[ \\t]+\\d+")

    string(REGEX REPLACE "^#define[ \\t]+PHYSFS_VER_MAJOR[ \\t]+(\\d+)" "\\1" PHYSFS_MAJOR "${physfs_major_version}")
    string(REGEX REPLACE "^#define[ \\t]+PHYSFS_VER_MAJOR[ \\t]+(\\d+)" "\\1" PHYSFS_MINOR "${physfs_minor_version}")
    string(REGEX REPLACE "^#define[ \\t]+PHYSFS_VER_MAJOR[ \\t]+(\\d+)" "\\1" PHYSFS_PATCH "${physfs_patch_version}")

    set(PHYSFS_VERSION_STRING "${PHYSFS_MAJOR}.${PHYSFS_MINOR}.${PHYSFS_PATCH}")
    
    unset(physfs_major_version)
    unset(physfs_minor_version)
    unset(physfs_patch_version)
  endif ()
endif()