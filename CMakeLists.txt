cmake_minimum_required(VERSION 3.10)
project(pong C)

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

add_executable(pong src/pong.c)
target_link_libraries(pong raylib)

add_custom_target(resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/resources
)
