cmake_minimum_required(VERSION 3.0.0)
project(ImageProcessing VERSION 0.1.0)

include(CTest)
enable_testing()

set(OpenCV_DIR "C:/opencv/build/x64/vc15/lib")
find_package(OpenCV REQUIRED) 
find_package(Threads REQUIRED)

add_executable(ImageProcessing main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(ImageProcessing ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})
