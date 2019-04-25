cmake_minimum_required(VERSION 2.8)

project(MapRenderer)
add_executable(${PROJECT_NAME} "main.cpp" "earcut.hpp")
target_link_libraries(${PROJECT_NAME} readosm SDL2 SDL2_gfx)
