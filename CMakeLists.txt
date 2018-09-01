cmake_minimum_required(VERSION 2.8.4)
project(Semaphore)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")
# Generate shared library libCyclicBarrier.so file.
ADD_LIBRARY(Semaphore SHARED Semaphore.cpp)

add_executable(Example1.out Example1.cpp)
add_executable(Example2.out Example2.cpp)
add_executable(Example3.out Example3.cpp)

target_link_libraries(Example1.out Semaphore pthread)
target_link_libraries(Example2.out Semaphore pthread)
target_link_libraries(Example3.out Semaphore pthread)

