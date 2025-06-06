cmake_minimum_required(VERSION 3.24.0)

project(RtspAnalyser.Streamers VERSION 0.0.1)

set(SOURCES
    src/Streamer.cpp
)

set(INCLUDES
    includes/DelNico/RtspAnalyser/Streamers/Streamer.h
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
    PRIVATE
        ${OpenCV_LIBS}
        Boost::asio
        fmt::fmt
        RtspAnalyser.Libs
        RtspAnalyser.Analyser
    PUBLIC
        Threads::Threads
)

