cmake_minimum_required(VERSION 3.24.0)

project(RtspAnalyser.Libs VERSION 0.0.1)

set(SOURCES
    src/ConditionalVariable.cpp
    src/Config.cpp
    src/Spinlock.cpp
)

set(INCLUDES
    includes/Nico/RtspAnalyser/Libs/Codec.h
    includes/Nico/RtspAnalyser/Libs/ConditionalVariable.h
    includes/Nico/RtspAnalyser/Libs/Config.h
    includes/Nico/RtspAnalyser/Libs/Spinlock.h
    includes/Nico/RtspAnalyser/Libs/Stream.h
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
        nlohmann_json::nlohmann_json
    PUBLIC
        Threads::Threads
)

