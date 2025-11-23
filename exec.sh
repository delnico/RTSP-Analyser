#!/bin/bash

export DISPLAY=:0

architecture=$(uname -m)

echo "Running for $architecture"
echo "Press q to exit"
echo "Press r to reload config"

if [ "$architecture" == "x86_64" ]; then
    ./build/x86_64/debug/RtspAnalyser/RtspAnalyser -c ./rtsp_config.json
else
    ./build/arm/debug/RtspAnalyser/RtspAnalyser -c ./rtsp_config.json -l ./log.txt
fi

