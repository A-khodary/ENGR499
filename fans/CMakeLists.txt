cmake_minimum_required (VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(RunFans)

find_package(Threads REQUIRED)

add_executable(run fansController.cpp jetsonGPIO.c)

target_link_libraries(run ${CMAKE_THREAD_LIBS_INIT})
