add_library(imgui STATIC)
target_sources(imgui
PRIVATE
    "${CMAKE_SOURCE_DIR}/extern/imgui/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/extern/imgui/imgui_draw.cpp"
    "${CMAKE_SOURCE_DIR}/extern/imgui/imgui_widgets.cpp"
    "${CMAKE_SOURCE_DIR}/extern/imgui/imgui_tables.cpp"
    "${CMAKE_SOURCE_DIR}/extern/imgui/imgui_demo.cpp"
INTERFACE
    "${CMAKE_SOURCE_DIR}/extern/imgui/backends/imgui_impl_opengl3.cpp"
    "${CMAKE_SOURCE_DIR}/extern/imgui/backends/imgui_impl_sdl2.cpp"
)

target_include_directories(imgui PUBLIC
    "${CMAKE_SOURCE_DIR}/extern/imgui"
    "${CMAKE_SOURCE_DIR}/extern/imgui/backends"
)
