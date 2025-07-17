cmake_minimum_required(VERSION 3.29)
project(Orbit C)

set(CMAKE_C_STANDARD 99)

add_executable(Orbit main.c
        defs.h
        newton.c
        newton_defs.h
        2_newton.c
        newton2_defs.h
        3_newton.c
        newton3_defs.h
        solar_system.c
        solar_system.c
        sol.h
        constants.h)
