cmake_minimum_required(VERSION 3.16)
project(loguru)
set(CMAKE_CXX_STANDARD 17)

add_library(loguru loguru.hpp loguru.cpp)
