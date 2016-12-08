cmake_minimum_required(VERSION 2.4)
project(DaedalusCompiler)

file(GLOB SRC
    *.cpp
    *.h
)


add_library(daedalusc STATIC ${SRC})
set_target_properties(daedalusc PROPERTIES LINKER_LANGUAGE C)
