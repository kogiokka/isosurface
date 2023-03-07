include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET FALSE)

if(NOT glm_FOUND)
    FetchContent_Declare(
        glm
        URL "https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.7z"
        URL_HASH MD5=c8342552801ebeb31497288192c4e793
    )
    FetchContent_MakeAvailable(glm)
endif()

if(NOT SDL2_FOUND)
    FetchContent_Declare(
        SDL2
        URL "https://github.com/libsdl-org/SDL/releases/download/release-2.26.3/SDL2-2.26.3.zip"
        URL_HASH MD5=bce2d8c92b264822d62ce6cf0e8241f9
    )
    FetchContent_MakeAvailable(SDL2)
endif()

add_subdirectory("glad")
add_subdirectory("imgui")
