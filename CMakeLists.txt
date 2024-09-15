cmake_minimum_required(VERSION 3.13.4)

project(RtspAnalyser VERSION 0.0.1)

# Compiler flags
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /VERBOSE")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /VERBOSE")

# Set debug build
set(CMAKE_BUILD_TYPE Debug)

# Set release build
#set(CMAKE_BUILD_TYPE Release)

# Set C++17
set(CMAKE_CXX_STANDARD 17)
# Link static lib as shared
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Dependencies
set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-linux/share/opencv4")
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)
include_directories(${GSTREAMER_INCLUDE_DIRS} ${GSTREAMER_APP_INCLUDE_DIRS})

find_package(Threads)

add_subdirectory(RtspAnalyser)

set_property(TARGET RtspAnalyser PROPERTY CXX_STANDARD 17)
