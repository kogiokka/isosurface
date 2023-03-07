cmake_minimum_required(VERSION 3.12.4)

project("Isosurface")

find_package(OpenGL REQUIRED)
find_package(glm QUIET)
find_package(SDL2 QUIET)

add_subdirectory("extern")
add_subdirectory("src/isosurface" "bin")
