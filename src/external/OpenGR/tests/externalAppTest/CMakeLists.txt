project(OpenGR-externalAppTest)
cmake_minimum_required(VERSION 3.3)

set (CMAKE_CXX_STANDARD 11)

find_package(OpenGR REQUIRED)
include_directories(${OpenGR_INCLUDE_DIR})
link_directories(${OpenGR_LIB_DIR})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenGR_LIBRARIES})

message (STATUS "OpenGR_INCLUDE_DIR: ${OpenGR_INCLUDE_DIR}")
message (STATUS "OpenGR_LIB_DIR    : ${OpenGR_LIB_DIR}")
message (STATUS "OpenGR_LIBRARIES  : ${OpenGR_LIBRARIES}")


# we also need eigen
find_package( Eigen3 REQUIRED )
include_directories( ${EIGEN3_INCLUDE_DIR} )

install( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .  )

