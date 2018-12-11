cmake_minimum_required(VERSION 3.8)

project(server)

# Setting the C++ standard directly
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the executable
add_executable(server main.cpp wrapsock.h)

# Add link libraries
target_link_libraries(server LINK_PRIVATE Ws2_32)
