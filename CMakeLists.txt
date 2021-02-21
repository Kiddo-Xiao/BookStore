cmake_minimum_required(VERSION 3.15)
project(code)

set(CMAKE_CXX_STANDARD 14)

add_executable(code  src/main.cpp src/main.h src/Bookstore.cpp src/Bookstore.h
        src/SquareLinkList.cpp src/SquareLinkList.h
        src/Commandrelated.cpp src/Commandrelated.h)