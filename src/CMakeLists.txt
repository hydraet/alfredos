cmake_minimum_required(VERSION 3.0)

project(AlfredOS LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-pthread")

# Define an executable
set(SRCS alfredos.cpp IThread.cpp MainThread.cpp Demo1Thread.cpp Demo2Thread.cpp)
add_executable(alfredos ${SRCS})
