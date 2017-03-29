cmake_minimum_required (VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(CameraThread)
set(CMAKE_MODULE_PATH ${CameraThread_SOURCE_DIR})

add_library(camera camera.cpp)

find_package(OpenCV REQUIRED)
find_package(ZBar0 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${ZBAR_INCLUDE_DIRS})

add_executable(cameraThread main.cpp camera.cpp)

target_link_libraries(cameraThread ${OpenCV_LIBS} ${ZBAR_LIBRARIES})
