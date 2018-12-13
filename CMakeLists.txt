cmake_minimum_required(VERSION 3.8)

project(server)

# Setting the C++ standard directly
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/network)

# Add the executable
add_executable(server main.cpp src/network/Server.hpp src/network/udp/Socket.hpp src/wrapsock.h)

# Add link libraries
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
target_link_libraries(server LINK_PRIVATE Ws2_32)
endif()
