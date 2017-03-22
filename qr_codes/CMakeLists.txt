cmake_minimum_required (VERSION 2.8)

project(img_proc)

set(CMAKE_MODULE_PATH ${img_proc_SOURCE_DIR})

find_package(OpenCV REQUIRED)
find_package(ZBar0 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${ZBAR_INCLUDE_DIRS})

add_executable(cv_img_proc img_proc.cpp)
#add_executable(cv_img_proc qr_proc.cpp)

target_link_libraries(cv_img_proc ${OpenCV_LIBS} ${ZBAR_LIBRARIES})
