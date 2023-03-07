add_library(imgui STATIC)
target_sources(imgui
PRIVATE
    "imgui.cpp"
    "imgui_draw.cpp"
    "imgui_widgets.cpp"
    "imgui_tables.cpp"
    "imgui_demo.cpp"
INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp"
)

target_include_directories(imgui PUBLIC
    "."
    "backends"
)

