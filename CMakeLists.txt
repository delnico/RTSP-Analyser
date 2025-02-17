cmake_minimum_required(VERSION 3.24.0)

project(RtspAnalyser VERSION 0.0.1)

# Set C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Link static lib as shared
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Dependencies
set(OpenCV_DIR "/usr/local/lib/cmake/opencv4")
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(Threads)
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS asio program_options)
find_package(fmt CONFIG REQUIRED)

# Add Libraries
add_subdirectory(RtspAnalyser.Libs)
add_subdirectory(RtspAnalyser.Streamers)
add_subdirectory(RtspAnalyser.Analyser)
add_subdirectory(RtspAnalyser.Motion)
add_subdirectory(RtspAnalyser.WatchdogLib)
add_subdirectory(RtspAnalyser)

set_property(TARGET RtspAnalyser PROPERTY CXX_STANDARD 20)
