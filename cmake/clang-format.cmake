file(GLOB_RECURSE
     ALL_CXX_SOURCE_FILES
     ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*/*.h
     )

find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
  add_custom_target(
    clang-format
    COMMAND ${CLANG_FORMAT}
    -i
    -style=file
    ${ALL_CXX_SOURCE_FILES}
    )
endif()
