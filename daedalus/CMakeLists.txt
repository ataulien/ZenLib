cmake_minimum_required(VERSION 2.4)
project(Daedalus)

file(GLOB SRC
    *.cpp
    *.h
)


add_library(daedalus STATIC ${SRC})
target_link_libraries(daedalus utils zenload)
set_target_properties(daedalus PROPERTIES LINKER_LANGUAGE C)
