cmake_minimum_required(VERSION 2.4)
project(ZenLoad)

file(GLOB SRC
    *.cpp
    *.h
)


add_library(zenload STATIC ${SRC})
target_link_libraries(zenload utils vdfs squish) 
set_target_properties(zenload PROPERTIES LINKER_LANGUAGE C)
