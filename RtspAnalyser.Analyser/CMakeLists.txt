cmake_minimum_required(VERSION 3.13.4)

project(RtspAnalyser.Analyser VERSION 0.0.1)

set(SOURCES
    src/HumanDetector.cpp
    src/IAnalyser.cpp
    src/Multiplexer.cpp
    src/OutputStream.cpp
    src/Viewer.cpp
)

set(INCLUDES
    includes/DelNico/RtspAnalyser/Analyser/HumanDetector.h
    includes/DelNico/RtspAnalyser/Analyser/IAnalyser.h
    includes/DelNico/RtspAnalyser/Analyser/Multiplexer.h
    includes/DelNico/RtspAnalyser/Analyser/OutputStream.h
    includes/DelNico/RtspAnalyser/Analyser/Viewer.h
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
        fmt::fmt
        libzmq
        cppzmq
        RtspAnalyser.Libs
        RtspAnalyser.Motion
    PUBLIC
        Threads::Threads
)

