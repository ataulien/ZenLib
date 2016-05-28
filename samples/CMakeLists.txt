cmake_minimum_required(VERSION 3.1)
project(Samples)

add_executable(vdf_unpack vdf_unpack.cpp)
target_link_libraries(vdf_unpack vdfs utils)

add_executable(zen_load zen_load.cpp)
target_link_libraries(zen_load zenload vdfs utils)

add_executable(content_load content_load.cpp)
target_link_libraries(content_load zenload vdfs utils)

add_executable(dat_load dat_load.cpp)
target_link_libraries(dat_load zenload vdfs utils daedalus)

add_executable(ou_load ou_load.cpp)
target_link_libraries(ou_load zenload vdfs utils)
