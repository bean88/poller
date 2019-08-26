cmake_minimum_required(VERSION 3.0)
set(PACKAGE_NAME "poller")
project(${PACKAGE_NAME} C CXX)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PACKAGE_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/epoll.c
        ${CMAKE_CURRENT_SOURCE_DIR}/main/tcp_session.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main/tcp_server.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main/main.cpp
        )
target_link_libraries(${PACKAGE_NAME} pthread)