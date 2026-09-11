cmake_minimum_required(VERSION 3.10)
project(Rasterizer)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

add_executable(rasterizer main.cpp render.cpp transform.cpp)

target_link_libraries(rasterizer PRIVATE SDL2::SDL2)