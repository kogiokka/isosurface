add_library(glad STATIC)
target_sources(glad PRIVATE "${CMAKE_SOURCE_DIR}/extern/glad/src/glad.c")
target_include_directories(glad PUBLIC "${CMAKE_SOURCE_DIR}/extern/glad/include")
