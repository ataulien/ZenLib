cmake_minimum_required(VERSION 3.10)
project(Utils)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(utils STATIC ${SRC})
