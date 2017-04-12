cmake_minimum_required(VERSION 3.5)

SET(DRIVE_SRCS
    main.cpp)

#include_directories(${CMAKE_SOURCE_DIR}/../RTIMULib)

include(${CMAKE_SOURCE_DIR}/../RTIMULib/CMakeLists.txt)

ADD_EXECUTABLE(IMUProgram ${DRIVE_SRCS})
TARGET_LINK_LIBRARIES(IMUProgram RTIMULib)

#INSTALL(TARGETS IMUProgram DESTINATION bin)
