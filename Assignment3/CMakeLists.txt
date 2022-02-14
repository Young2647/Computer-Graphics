cmake_minimum_required(VERSION 3.16)
project(CS171-hw3 LANGUAGES C CXX)

add_subdirectory(libs)

find_package(OpenMP)

add_subdirectory(src)