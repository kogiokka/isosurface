find_package(SDL2 QUIET)

if(SDL2_FOUND)
    return()
endif()

FetchContent_Declare(
    SDL2
    URL "https://github.com/libsdl-org/SDL/releases/download/release-2.26.3/SDL2-2.26.3.zip"
    URL_HASH MD5=bce2d8c92b264822d62ce6cf0e8241f9
)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_MakeAvailable(SDL2)
