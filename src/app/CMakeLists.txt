include("${CMAKE_SOURCE_DIR}/cmake/deps.cmake")

add_executable(isosurface)

set_target_properties(isosurface
PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

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

target_sources(isosurface
PRIVATE
    "Camera.cpp"
    "Isosurface.cpp"
    "main.cpp"
    "MainWindow.cpp"
    "MarchingCubesTables.cpp"
    "MarchingTetrahedraTables.cpp"
    "Model.cpp"
    "SDLOpenGLWindow.cpp"
    "Shader.cpp"
    "Util.cpp"
    "VolumeData.cpp"
)

target_include_directories(isosurface
PRIVATE
    "include/"
)

target_link_libraries(isosurface
PRIVATE
    OpenGL::GL
    SDL2::SDL2
    glm::glm
    glad
    imgui
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
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

if (WIN32)
    add_custom_command(TARGET isosurface POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL2::SDL2>"
        "$<TARGET_FILE_DIR:isosurface>"
    )
endif()

add_custom_command(TARGET isosurface POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/shader"
    "$<TARGET_FILE_DIR:isosurface>/shader"
COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/fonts"
    "$<TARGET_FILE_DIR:isosurface>/fonts"
COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/data"
    "$<TARGET_FILE_DIR:isosurface>/data"
VERBATIM
)
