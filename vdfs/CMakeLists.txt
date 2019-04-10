cmake_minimum_required(VERSION 3.10)
project(VDFS)

find_package(PhysFS 3 REQUIRED)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(vdfs STATIC ${SRC})

# Remove WIN-API min and max macros since they mess up std::min/max
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX")
endif()

target_link_libraries(vdfs PRIVATE PhysFS::PhysFS)
set_target_properties(vdfs PROPERTIES LINKER_LANGUAGE C)
target_include_directories(vdfs PUBLIC ..)
