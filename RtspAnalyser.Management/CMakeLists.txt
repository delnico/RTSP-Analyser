cmake_minimum_required(VERSION 3.24.0)

project(RtspAnalyser.Management VERSION 0.0.1)

set(SOURCES
    src/StreamManager.cpp
)

set(INCLUDES
    includes/DelNico/RtspAnalyser/Management/StreamManager.h
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
        Boost::asio
        fmt::fmt
        RtspAnalyser.Analyser
        RtspAnalyser.Libs
        RtspAnalyser.Motion
        RtspAnalyser.Streamers
        RtspAnalyser.WatchdogLib
    PUBLIC
        Threads::Threads
)

