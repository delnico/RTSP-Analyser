#!/bin/bash

architecture=($(uname -m))

echo "Building for $architecture"

if [ "$architecture" == "x86_64" ]; then
    cmake --preset x86_64-linux-debug
    cmake --build --preset x86_64-linux-debug -j $(nproc)
    cp rtsp_config.json ./build/x86_64/debug/RtspAnalyser/config.json
else
    cmake --preset arm-linux-debug
    cmake --build --preset arm-linux-debug -j $(nproc)
    cp rtsp_config.json ./build/arm/debug/RtspAnalyser/config.json
fi

