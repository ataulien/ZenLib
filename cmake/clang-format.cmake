file(GLOB_RECURSE
     ALL_CXX_SOURCE_FILES
     ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.h
     ${CMAKE_CURRENT_SOURCE_DIR}/vdfs/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vdfs/*.h
     ${CMAKE_CURRENT_SOURCE_DIR}/zenload/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/zenload/*.h
     ${CMAKE_CURRENT_SOURCE_DIR}/daedalus/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/daedalus/*.h
     )

find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
  add_custom_target(
    "clang-format-${PROJECT_NAME}"
    COMMAND ${CLANG_FORMAT}
    -i
    -style=file
    ${ALL_CXX_SOURCE_FILES}
    )
endif()
