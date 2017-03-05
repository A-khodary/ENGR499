cmake_minimum_required (VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(CameraThread)

add_library(camera camera.cpp)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cameraThread main.cpp camera.cpp)

target_link_libraries(cameraThread ${OpenCV_LIBS})
