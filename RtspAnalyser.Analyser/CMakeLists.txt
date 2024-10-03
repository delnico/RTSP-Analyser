cmake_minimum_required(VERSION 3.13.4)

project(RtspAnalyser.Analyser VERSION 0.0.1)

set(SOURCES
)

set(INCLUDES
    includes/Nico/RtspAnalyser/Analyser/IAnalyser.h
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

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Threads::Threads
)

