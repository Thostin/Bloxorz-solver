cmake_minimum_required(VERSION 3.7)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(bloxorz LANGUAGES C)

add_subdirectory(BloxorzAlgorithm)
add_executable(${PROJECT_NAME} main.c menu.c)

add_compile_options("-Wall -Werror -Wpedantic -Wextra -O3 ")

target_link_libraries(${PROJECT_NAME} PUBLIC bloxorz_algorithm)

install(TARGETS ${PROJECT_NAME} DESTINATION /bin)

# target_include_directories(${PROJECT_NAME} include/) install(TARGETS bloxorz
# DESTINATION /bn)
