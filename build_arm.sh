#!/bin/bash

cmake --preset arm-linux-debug

cmake --build --preset arm-linux-debug -j $(nproc)

cp rtsp_config.json ./build/arm/debug/RtspAnalyser/config.json
