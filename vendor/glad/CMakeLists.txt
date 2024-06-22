cmake_minimum_required(VERSION 3.26)

project(Glad)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_library(glad
        src/glad.c
        include/glad/glad.h
        include/KHR/khrplatform.h)