cmake_minimum_required(VERSION 3.4.1)
add_library(translib SHARED translib.c)
target_link_libraries(translib)
