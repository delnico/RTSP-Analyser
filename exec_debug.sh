#!/bin/bash

architecture=$(uname -m)

echo "Running for $architecture"
echo "Press q to exit"

ulimit -c unlimited

export GST_PLUGIN_SYSTEM_PATH="/home/nico/project/RTSP-Analyser/build/x86_64/debug/vcpkg_installed/x64-linux-cascadelake/plugins/gstreamer"
export GST_PLUGIN_PATH="/home/nico/project/RTSP-Analyser/build/x86_64/debug/vcpkg_installed/x64-linux-cascadelake/plugins/gstreamer"
export GST_REGISTRY="/tmp/gst_registry_vcpkg_debug.bin"
./build/x86_64/debug/RtspAnalyser/RtspAnalyser -c rtsp_config.json

