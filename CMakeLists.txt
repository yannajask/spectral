cmake_minimum_required(VERSION 4.0.2)

project(spectral)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

set(CXX_FLAGS "-Wall -g -O3")

add_executable(spectral src/main.cpp)
include_directories(src)