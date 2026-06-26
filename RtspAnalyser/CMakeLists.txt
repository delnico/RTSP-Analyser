
set(INCLUDES
    includes/DelNico/RtspAnalyser/StreamAnalyserHandler.h
)

set(SOURCES
    src/StreamAnalyserHandler.cpp
    src/main.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
    ${INCLUDES}
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
    $<INSTALL_INTERFACE:include>
)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
)

link_directories( ${OpenCV_LIB_DIR} )

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OpenCV_LIBS}
    Boost::asio
    Boost::program_options
    libzmq
    cppzmq
    RtspAnalyser.Libs
    RtspAnalyser.Receivers
    RtspAnalyser.Analyser
    RtspAnalyser.Motion
    RtspAnalyser.Watchdog
)
