cmake_minimum_required(VERSION 3.12.4)

project(isosurface LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -D_GLIBCXX_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

include_directories(include/)
include_directories(ext/imgui/)
include_directories(ext/glad/include)
include_directories(ext/glm/include)

add_subdirectory(ext/glm)

file(GLOB sourceFiles CONFIGURE_DEPENDS
    "src/*.cc"
    "src/*.cpp"
    "ext/imgui/*.cpp"
    "ext/glad/src/*.c"
)

add_executable(${PROJECT_NAME}
    ${sourceFiles}
)

target_link_libraries(${PROJECT_NAME}
    SDL2::SDL2-static
    SDL2::SDL2main
    glm
    pthread
    m
)
