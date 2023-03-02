add_library(imgui STATIC)
target_sources(imgui
PRIVATE
    "ext/imgui/imgui.cpp"
    "ext/imgui/imgui_draw.cpp"
    "ext/imgui/imgui_widgets.cpp"
    "ext/imgui/imgui_tables.cpp"
    "ext/imgui/imgui_demo.cpp"
INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/imgui/backends/imgui_impl_opengl3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/imgui/backends/imgui_impl_sdl2.cpp"
)

target_include_directories(imgui PUBLIC
    "ext/imgui"
    "ext/imgui/backends"
)
