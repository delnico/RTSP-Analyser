
set(SOURCES
    src/main.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
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
    RtspAnalyser.Libs
    RtspAnalyser.Streamers
    RtspAnalyser.Analyser
    RtspAnalyser.Motion
    RtspAnalyser.WatchdogLib
)
