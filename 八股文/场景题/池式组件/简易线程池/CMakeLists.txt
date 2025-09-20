cmake_minimum_required(VERSION 3.31)
project(thread_pool_project)

set(CMAKE_CXX_STANDARD 20)

add_executable(thread_pool_main
        main.cpp
        thread_pool_test.cpp
        thread_pool_test.h
        thread_pool.cpp
        thread_pool.h)
