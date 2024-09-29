cmake_minimum_required(VERSION 3.8)

project(atende)

# Setup C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Setup include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/network)

# Add executable
add_executable(posix_server src/network/posix/Server.cpp
    src/network/posix/Session.cpp

    src/core/ExecutionFactory.cpp
    src/core/ExecutionPool.cpp
    src/core/TaskProviderList.cpp
    src/core/ThreadWorker.cpp
    src/core/CancelTokenProvider.cpp)

add_executable(posix_client src/network/posix/Client.cpp)

add_executable(parallel_server src/blocking_tcp_echo_server.cpp
    src/network/parallel/Server.cpp
    src/network/parallel/Session.cpp
    src/network/protocol/Command.cpp
    src/network/protocol/CommandOne.cpp
    src/network/protocol/CommandTwo.cpp

    src/core/ExecutionFactory.cpp
    src/core/ExecutionPool.cpp
    src/core/TaskProviderList.cpp
    src/core/ThreadWorker.cpp
    src/core/CancelTokenProvider.cpp)

add_executable(parallel_client src/blocking_tcp_echo_client.cpp
    src/network/parallel/Client.cpp
    src/network/protocol/Command.cpp
    src/network/protocol/CommandOne.cpp
    src/network/protocol/CommandTwo.cpp

    src/core/ExecutionFactory.cpp
    src/core/ExecutionPool.cpp
    src/core/TaskProviderList.cpp
    src/core/ThreadWorker.cpp
    src/core/CancelTokenProvider.cpp)

add_executable(async_server src/async_tcp_echo_server.cpp
    src/network/async/Server.cpp
    src/network/async/Session.cpp

    src/core/ExecutionFactory.cpp
    src/core/ExecutionPool.cpp
    src/core/TaskProviderList.cpp
    src/core/ThreadWorker.cpp
    src/core/CancelTokenProvider.cpp)
