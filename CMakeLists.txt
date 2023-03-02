cmake_minimum_required(VERSION 3.12.4)

project("Isosurface")

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(isosurface)

set_target_properties(isosurface PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

include("cmake/glad.cmake")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(isosurface PRIVATE $<$<CONFIG:Debug>:-Wall -Wextra -pedantic-errors -ggdb>)
    target_compile_options(isosurface PRIVATE $<$<CONFIG:Release>:-Wall -Wextra -pedantic-errors>)
    target_compile_definitions(isosurface PRIVATE $<$<CONFIG:Debug>:_GLIBCXX_DEBUG>)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(isosurface PRIVATE $<$<CONFIG:Debug>:-Wall -Wextra -pedantic-errors -ggdb>)
    target_compile_options(isosurface PRIVATE $<$<CONFIG:Release>:-Wall -Wextra -pedantic-errors>)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(isosurface PRIVATE $<$<CONFIG:Debug>:/W4 /permissive->)
    target_compile_options(isosurface PRIVATE $<$<CONFIG:Release>:/W4 /permissive->)
endif()

include_directories(include/)
include_directories(ext/imgui/)

add_subdirectory(ext/glm)

file(GLOB sourceFiles CONFIGURE_DEPENDS
    "src/*.cc"
    "src/*.cpp"
    "ext/imgui/*.cpp"
)

target_sources(isosurface PRIVATE ${sourceFiles})

target_link_libraries(isosurface PRIVATE
    OpenGL::GL
    SDL2::SDL2
    glm::glm
    glad
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(isosurface PRIVATE SDL2::SDL2main)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1)
        target_link_libraries(isosurface PRIVATE stdc++fs) # C++ filesystem
    endif()
    if (MINGW)
        # https://github.com/msys2/MINGW-packages/issues/6380
        add_definitions(-DSDL_MAIN_HANDLED)
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        target_link_libraries(isosurface PRIVATE c++fs) # C++ filesystem
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(isosurface PRIVATE SDL2::SDL2main)
endif()

add_custom_command(TARGET isosurface POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shader"
        "$<TARGET_FILE_DIR:isosurface>/shader"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/res"
        "$<TARGET_FILE_DIR:isosurface>/res"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/models"
        "$<TARGET_FILE_DIR:isosurface>/models"
    VERBATIM
)
