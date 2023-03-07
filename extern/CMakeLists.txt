include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET FALSE)

add_subdirectory("glm")
add_subdirectory("SDL2")
add_subdirectory("glad")
add_subdirectory("imgui")
