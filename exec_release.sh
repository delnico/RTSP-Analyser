#!/bin/bash

architecture=$(uname -m)

echo "Running for $architecture"
echo "Press q to exit"

export GST_PLUGIN_SYSTEM_PATH="/home/nico/project/RTSP-Analyser/build/x86_64/release/vcpkg_installed/x64-linux-cascadelake/plugins/gstreamer"
export GST_PLUGIN_PATH="/home/nico/project/RTSP-Analyser/build/x86_64/release/vcpkg_installed/x64-linux-cascadelake/plugins/gstreamer"
export GST_REGISTRY="/tmp/gst_registry_vcpkg.bin"
./build/x86_64/release/RtspAnalyser/RtspAnalyser -c rtsp_config.json

