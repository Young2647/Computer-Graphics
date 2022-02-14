cmake_minimum_required(VERSION 3.16)
project(CS171-hw4 LANGUAGES C CXX)

find_package(OpenMP)

add_subdirectory(libs)
add_subdirectory(src)