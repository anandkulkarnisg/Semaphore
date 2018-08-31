cmake_minimum_required(VERSION 2.8.4)
project(Semaphore)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")
# Generate shared library libCyclicBarrier.so file.
ADD_LIBRARY(Semaphore SHARED Semaphore.cpp)

add_executable(Example1.out Example1.cpp)
target_link_libraries(Example1.out Semaphore pthread)

