#!/bin/bash

cmake --preset x86_64-linux-debug

cmake --build --preset x86_64-linux-debug

cp rtsp_config.json ./build/x86_64/debug/RtspAnalyser/config.json
