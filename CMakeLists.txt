cmake_minimum_required(VERSION 4.0.2)

project(spectral)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)
include_directories(/opt/homebrew/include)
include_directories(extern/lodepng)

link_directories(/opt/homebrew/lib)

set(CXX_FLAGS "-Wall -g -O3")

add_executable(spectral 
    src/main.cc
    src/geometry/Sphere.cc
    src/geometry/Triangle.cc
    src/geometry/Scene.cc
    src/geometry/Mesh.cc
    src/geometry/AABB.cc
    src/geometry/BVH.cc
    extern/lodepng/lodepng.cpp
)

include_directories(src)