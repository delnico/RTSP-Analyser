cmake_minimum_required(VERSION 3.13.4)

project(RtspAnalyser.Streamer VERSION 0.0.1)

set(SOURCES
    src/Streamer.cpp
)

set(INCLUDES
    includes/Nico/RtspAnalyser/Streamer/Streamer.h
)

add_library(${PROJECT_NAME}
    ${SOURCES}
    ${INCLUDES}
)

link_directories( ${OpenCV_LIB_DIR} )

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Threads::Threads
        RtspAnalyser.Libs
        RtspAnalyser.Analyser
)

