cmake_minimum_required(VERSION 3.0)

project(usb_camera_test)

#set(CUDA_USE_STATIC_CUDA_RUNTIME "OFF")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(usb_camera usbcamera.cpp)

target_link_libraries(usb_camera ${OpenCV_LIBS})
