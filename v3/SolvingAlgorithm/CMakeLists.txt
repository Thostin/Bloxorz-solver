cmake_minimum_required(VERSION 3.10)
add_library(bloxorz_algorithm SHARED algorithm.c glob_var.c)
target_include_directories(bloxorz_algorithm
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

install(TARGETS bloxorz_algorithm DESTINATION /usr/lib/)

