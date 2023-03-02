find_package(glm QUIET)

if(glm_FOUND)
    return()
endif()

FetchContent_Declare(
    glm
    URL "https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.7z"
    URL_HASH MD5=c8342552801ebeb31497288192c4e793
)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_MakeAvailable(glm)
