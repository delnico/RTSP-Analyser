cmake_minimum_required(VERSION 3.13.4)

project(RtspAnalyser VERSION 0.0.1)

# Compiler flags
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /VERBOSE")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /VERBOSE")
set(CMAKE_CXX_STANDARD 17)
# Link static lib as shared
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Dependencies
set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-linux/share/opencv4")
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(Threads)

add_subdirectory(RtspAnalyser)

set_property(TARGET RtspAnalyser PROPERTY CXX_STANDARD 17)
