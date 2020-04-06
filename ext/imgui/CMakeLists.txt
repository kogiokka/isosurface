include_directories(./)

file(GLOB SOURCES CONFIGURE_DEPENDS
  *.h
  *.cpp
)

add_library(imgui STATIC
  ${SOURCES}
)
