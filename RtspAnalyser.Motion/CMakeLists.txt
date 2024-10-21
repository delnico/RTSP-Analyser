cmake_minimum_required(VERSION 3.24.0)

project(RtspAnalyser.Motion VERSION 0.0.1)

set(SOURCES
    src/MotionDetector.cpp
    src/MotionEvent.cpp
)

set(INCLUDES
    includes/Nico/RtspAnalyser/Motion/MotionDetector.h
    includes/Nico/RtspAnalyser/Motion/MotionEvent.h
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
    PRIVATE
        ${OpenCV_LIBS}
        RtspAnalyser.Libs
        RtspAnalyser.Analyser
    PUBLIC
        Threads::Threads
)

