add_library(glad STATIC)
target_sources(glad PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/src/glad.c")
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/include")
