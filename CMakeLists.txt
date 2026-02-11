cmake_minimum_required(VERSION 4.0)
project(Relativity)

set(CMAKE_CXX_STANDARD 20)

add_executable(Relativity src/main.cpp
        src/Game.cpp
        src/Game.h)
