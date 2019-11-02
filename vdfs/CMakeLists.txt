cmake_minimum_required(VERSION 3.9)
project(VDFS)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(vdfs STATIC ${SRC})

# Remove WIN-API min and max macros since they mess up std::min/max
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX")
endif()


# PhysFS's CMakeLists does not follow modern CMake guidelines
target_include_directories(vdfs PRIVATE ${physfs_SOURCE_DIR}/src)

target_link_libraries(vdfs PRIVATE PhysFS)
set_target_properties(vdfs PROPERTIES LINKER_LANGUAGE C)
target_include_directories(vdfs PUBLIC ..)
