cmake_minimum_required(VERSION 3.21)
project(test)

set(CMAKE_CXX_STANDARD 20)

add_executable(test main.cpp)

target_include_directories(test PRIVATE "C:/Program Files/Git/vcpkg/installed/x64-windows/include")
