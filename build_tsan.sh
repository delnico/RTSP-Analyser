#!/bin/bash

architecture=($(uname -m))

echo "Building for $architecture debug_asan"

cmake --preset debug_tsan
cmake --build --preset debug_tsan
cp rtsp_config.json ./build/x86_64/debug_tsan/RtspAnalyser/config.json


