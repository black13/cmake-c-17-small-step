cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(small VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set build type to Debug
set(CMAKE_BUILD_TYPE Debug)

# Add an executable
add_executable(small main.cpp)
