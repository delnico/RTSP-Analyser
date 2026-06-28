#!/bin/bash

echo "Building for release x86-64"

cmake --preset x86_64-linux-release
cmake --build --preset x86_64-linux-release -j $(nproc)
cp rtsp_config.json ./build/x86_64/release/RtspAnalyser/config.json

