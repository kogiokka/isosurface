cmake_minimum_required(VERSION 3.12.4)

project(isosurface LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)

if (SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS})
    message(STATUS "SDL2 Include Path: ${SDL2_INCLUDE_DIRS}")
    message(STATUS "SDL2 Libraries: ${SDL2_LIBRARIES}")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -D_GLIBCXX_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

include_directories(include/)
include_directories(ext/glad/include)
# include_directories(ext/SDL2/include)
include_directories(ext/imgui)
include_directories(ext/glm/include)

add_subdirectory(ext/glad)
add_subdirectory(ext/glm)
add_subdirectory(ext/imgui)

# add_library(SDL2-static INTERFACE IMPORTED)
# add_library(SDL2main INTERFACE IMPORTED)
# set_property(TARGET SDL2-static PROPERTY INTERFACE_LINK_LIBRARIES "${PROJECT_SOURCE_DIR}/lib/libSDL2.a")
# set_property(TARGET SDL2main PROPERTY INTERFACE_LINK_LIBRARIES "${PROJECT_SOURCE_DIR}/lib/libSDL2main.a")

file(GLOB sourceFiles CONFIGURE_DEPENDS
    src/*.cc
)

add_executable(${PROJECT_NAME}
    ${sourceFiles}
)

target_link_libraries(${PROJECT_NAME}
    glad
    SDL2::SDL2main
    SDL2::SDL2-static
    glm
    imgui
    pthread
    m
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_link_libraries(${PROJECT_NAME} dl)
endif()
