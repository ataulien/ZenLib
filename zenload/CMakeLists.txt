cmake_minimum_required(VERSION 3.9)
project(ZenLoad)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(zenload STATIC ${SRC})
target_link_libraries(zenload PRIVATE utils vdfs squish)
set_target_properties(zenload PROPERTIES LINKER_LANGUAGE C)
target_include_directories(zenload PUBLIC ..)
