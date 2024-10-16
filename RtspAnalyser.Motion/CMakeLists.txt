cmake_minimum_required(VERSION 3.24.0)

project(RtspAnalyser.Motion VERSION 0.0.1)

set(SOURCES
    src/MotionDetector.cpp
)

set(INCLUDES
    includes/Nico/RtspAnalyser/Motion/MotionDetector.h
)

add_library(${PROJECT_NAME}
    ${SOURCES}
    ${INCLUDES}
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Threads::Threads
        ${OpenCV_LIBS}
        RtspAnalyser.Libs
        RtspAnalyser.Analyser
)

