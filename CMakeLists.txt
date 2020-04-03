cmake_minimum_required(VERSION 3.10)

project(isosurface LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3")

find_package(SDL2 REQUIRED)

include_directories(include/)
include_directories(ext/glad/include)
include_directories(ext/glm/include)

add_subdirectory(ext/glad)
add_subdirectory(ext/glm)

add_executable(${PROJECT_NAME}
  src/main.cc
  src/camera.cc
  src/scene.cc
  src/shader.cc
  src/constants.cc
  src/isosurface.cc
  src/model.cc
)

target_link_libraries(${PROJECT_NAME}
  glad
  SDL2
  glm
)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  target_link_libraries(${PROJECT_NAME} dl)
endif()


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
