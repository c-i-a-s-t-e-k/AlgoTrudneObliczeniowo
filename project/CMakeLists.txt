cmake_minimum_required(VERSION 3.27)
project(project)

set(CMAKE_CXX_STANDARD 17)

add_executable(project main.cpp
        carPark.h
        carPark.cpp
        Point.cpp
        map.cpp
        Point.h
)
