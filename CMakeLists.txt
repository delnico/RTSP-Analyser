cmake_minimum_required(VERSION 3.24.0)

project(RtspAnalyser VERSION 0.0.1)

# Set C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Link static lib as shared
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# set asan debug flags
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)

if(ENABLE_ASAN)
    add_compile_options(
        -fsanitize=address
        -fsanitize=undefined
        -fno-omit-frame-pointer
        -g
        -O1
    )

    add_link_options(
        -fsanitize=address
        -fsanitize=undefined
    )
endif()

option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)

if(ENABLE_TSAN)
    add_compile_options(
        -fsanitize=thread
        -g
        -O1
    )

    add_link_options(
        -fsanitize=thread
    )
endif()

# Dependencies
#set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-linux/share/opencv4")
set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-linux-cascadelake/share/opencv4")
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(Threads)
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS asio program_options)
find_package(fmt CONFIG REQUIRED)
find_package(ZeroMQ CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)
find_package(mailio CONFIG REQUIRED)
find_package(onnxruntime CONFIG REQUIRED)

# Add Libraries
add_subdirectory(RtspAnalyser.Analyser)
add_subdirectory(RtspAnalyser.Management)
add_subdirectory(RtspAnalyser.Motion)
add_subdirectory(RtspAnalyser.Libs)
add_subdirectory(RtspAnalyser.Receivers)
add_subdirectory(RtspAnalyser.WatchdogLib)
add_subdirectory(RtspAnalyser)

set_property(TARGET RtspAnalyser PROPERTY CXX_STANDARD 20)
