cmake_minimum_required (VERSION 2.8)

project(img_proc)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_img_proc img_proc.cpp)

target_link_libraries(cv_img_proc ${OpenCV_LIBS})