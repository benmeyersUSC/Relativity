cmake_minimum_required(VERSION 4.0)
project(Relativity)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(SDL3)

add_executable(Relativity src/main.cpp
        src/Game.cpp
        src/Game.h
        src/Math.h
        src/Actor.cpp
        src/Actor.h
        src/Paddle.cpp
        src/Paddle.h
        src/Transform.cpp
        src/Transform.h
        src/SpacetimePlot.cpp
        src/SpacetimePlot.h
)

target_link_libraries(Relativity PRIVATE SDL3::SDL3)
