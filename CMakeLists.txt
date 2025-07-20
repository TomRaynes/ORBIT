cmake_minimum_required(VERSION 3.29)
project(Orbit C)

set(CMAKE_C_STANDARD 99)

include_directories(/usr/local/include/SDL2)
link_directories(/usr/local/lib)

add_executable(Orbit main.c
        solar_system.c
        sol.h
        constants.h
)

target_link_libraries(Orbit SDL SDL_image)