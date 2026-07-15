#!/bin/bash

architecture=($(uname -m))

echo "Building for $architecture debug_asan"

cmake --preset debug_asan
cmake --build --preset debug_asan
cp rtsp_config.json ./build/x86_64/debug_asan/RtspAnalyser/config.json


