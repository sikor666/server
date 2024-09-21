cmake_minimum_required(VERSION 3.8)

project(atende)

# Setup C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Setup include directories
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/network)

# Add executable
add_executable(server main.cpp)
