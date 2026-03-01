cmake_minimum_required(VERSION 3.24.0)

project(RtspAnalyser.Watchdog VERSION 0.0.1)

set(SOURCES
    src/Watchdog.cpp
)

set(INCLUDES
    includes/DelNico/RtspAnalyser/Watchdog/Watchdog.h
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
        Boost::asio
        RtspAnalyser.Analyser
        RtspAnalyser.Motion
        RtspAnalyser.Streamers
        RtspAnalyser.Libs
    PUBLIC
        Threads::Threads
)

