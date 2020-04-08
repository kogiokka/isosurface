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

add_subdirectory(ext/SDL2)
add_subdirectory(ext/glad)
add_subdirectory(ext/glm)

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
  SDL2-static
  glm
  imgui
)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  target_link_libraries(${PROJECT_NAME} dl)
endif()


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
