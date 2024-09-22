cmake_minimum_required(VERSION 3.8)

project(atende)

# Setup C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Setup include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext)

# Add executable
add_executable(async_tcp_echo_server async_tcp_echo_server.cpp)
