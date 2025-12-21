cmake_minimum_required(VERSION 4.0.2)

project(spectral)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)
include_directories(/opt/homebrew/include)
include_directories(extern/lodepng)

link_directories(/opt/homebrew/lib)

set(OpenMP_C_FLAGS "-fopenmp")
set(OpenMP_CXX_FLAGS "-fopenmp -Wall -g -O3 -march=native")
set(OpenMP_C_LIB_NAMES "omp")
set(OpenMP_CXX_LIB_NAMES "omp")
set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/llvm/lib/libomp.dylib")

find_package(OpenMP REQUIRED)

add_executable(spectral 
    src/main.cc
    src/geometry/Sphere.cc
    src/geometry/Triangle.cc
    src/geometry/Scene.cc
    src/geometry/Mesh.cc
    src/geometry/AABB.cc
    src/geometry/BVH.cc
    src/geometry/Transform.cc
    src/materials/Dielectric.cc
    extern/lodepng/lodepng.cpp
)

target_link_libraries(spectral PRIVATE OpenMP::OpenMP_CXX)
include_directories(src)