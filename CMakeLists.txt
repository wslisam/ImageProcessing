cmake_minimum_required(VERSION 3.0)
project(IMV_Project)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(IMV_project main.cpp  processing.cpp)
target_link_libraries(IMV_project ${OpenCV_LIBS})