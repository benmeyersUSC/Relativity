cmake_minimum_required(VERSION 3.20)
project(Relativity)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(SDL3)
add_subdirectory(SDL3_image)


add_executable(Relativity src/main.cpp
        src/Game.cpp
        src/Game.h
        src/Math.h
        src/Actor.cpp
        src/Actor.h
        src/Player.cpp
        src/Player.h
        src/Transform.cpp
        src/Transform.h
        src/SpacetimeComponent.cpp
        src/SpacetimeComponent.h
        src/Component.cpp
        src/Component.h
        src/ImageComponent.cpp
        src/ImageComponent.h
        src/DrawComponent.cpp
        src/DrawComponent.h
        src/ReferenceActor.cpp
        src/ReferenceActor.h
        src/Line.cpp
        src/Line.h
        src/SpacetimeDisplay.cpp
        src/SpacetimeDisplay.h
        src/SpacetimeReference.cpp
        src/SpacetimeReference.h
        src/SpacetimePlayer.cpp
        src/SpacetimePlayer.h
)

target_link_libraries(Relativity PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

# Copy assets next to the executable so relative paths work everywhere
add_custom_command(TARGET Relativity POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/Assets
        $<TARGET_FILE_DIR:Relativity>/Assets
)

if(EMSCRIPTEN)
    set_target_properties(Relativity PROPERTIES SUFFIX ".html")
    target_link_options(Relativity PRIVATE
            --preload-file ${CMAKE_SOURCE_DIR}/src/Assets@/Assets
            --shell-file ${CMAKE_SOURCE_DIR}/web/shell.html
            -sALLOW_MEMORY_GROWTH=1
            -sGL_ENABLE_GET_PROC_ADDRESS=1
    )
endif()
