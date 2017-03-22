cmake_minimum_required (VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(ShapeRecognize)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(shape shape.cpp)

target_link_libraries(shape ${OpenCV_LIBS})
