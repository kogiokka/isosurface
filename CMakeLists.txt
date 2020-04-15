cmake_minimum_required(VERSION 3.12.4)

project(isosurface LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# find_package(SDL2 REQUIRED)

include_directories(include/)
include_directories(ext/glad/include)
include_directories(ext/glm/include)
include_directories(ext/imgui)

# add_subdirectory(ext/SDL2)
add_subdirectory(ext/glad)
add_subdirectory(ext/glm)

add_library(SDL2-static INTERFACE IMPORTED)
add_library(SDL2main INTERFACE IMPORTED)

set_property(TARGET SDL2-static PROPERTY INTERFACE_LINK_LIBRARIES "${PROJECT_SOURCE_DIR}/lib/libSDL2.a")
set_property(TARGET SDL2main PROPERTY INTERFACE_LINK_LIBRARIES "${PROJECT_SOURCE_DIR}/lib/libSDL2main.a")

include_directories(ext/SDL2/include)
add_subdirectory(ext/imgui)

file(GLOB SOURCES CONFIGURE_DEPENDS
  src/*.cc
)

add_executable(${PROJECT_NAME}
  ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
  glad
  SDL2main
  SDL2-static
  glm
  imgui
  pthread
  m
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  target_link_libraries(${PROJECT_NAME} dl)
endif()


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
