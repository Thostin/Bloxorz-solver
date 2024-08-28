cmake_minimum_required(VERSION 3.7)

# add_subdirectory(Solving) add_subdirectory(GlobalVariables)

add_library(
  bloxorz_algorithm STATIC Solving/algorithm.c GlobalVariables/glob_var.c
                           Playing/play_bloxorz.c Screen/screen.c)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_compile_options("-Wall -Werror -Wpedantic -O3")
# add_link_options("-lSDL2")

target_include_directories(bloxorz_algorithm
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(bloxorz_algorithm ${SDL2_LIBRARIES})

# target_link_libraries(${PROJECT_NAME} PRIVATE SDL2main SDL2 SDL2_image)

install(TARGETS bloxorz_algorithm DESTINATION /usr/lib/)
