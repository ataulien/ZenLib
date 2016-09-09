cmake_minimum_required(VERSION 2.4)
project(Utils)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(utils STATIC ${SRC})
