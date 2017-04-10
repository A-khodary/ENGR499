cmake_minimum_required (VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(JetsonRobot)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(OpenCV REQUIRED)
find_package(ZBar0 REQUIRED)

include(${CMAKE_SOURCE_DIR}/lib/RTIMULib/CMakeLists.txt)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${ZBAR_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

# Way to explicitly find sources
#set(SOURCES
#  ${CMAKE_SOURCE_DIR}/src/main.cpp
#  ${CMAKE_SOURCE_DIR}/src/camera.cpp
#  )

# Way to implicitly find sources
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(run ${SOURCES})

target_link_libraries(run ${OpenCV_LIBS} ${ZBAR_LIBRARIES} RTIMULib)
#target_link_libraries(run ${OpenCV_LIBS} ${ZBAR_LIBRARIES} ${RTIMU_LIBS})

#INSTALL(TARGETS run DESTINATION bin)
