#!/bin/bash

architecture=$(uname -m)

echo "Running for $architecture"
echo "Press q to exit"

ulimit -c unlimited

export GST_PLUGIN_SYSTEM_PATH="/home/nico/project/RTSP-Analyser/build/x86_64/debug_tsan/vcpkg_installed/x64-linux-cascadelake/plugins/gstreamer"
export GST_PLUGIN_PATH="/home/nico/project/RTSP-Analyser/build/x86_64/debug_tsan/vcpkg_installed/x64-linux-cascadelake/plugins/gstreamer"
export GST_REGISTRY="/tmp/gst_registry_vcpkg_debug_tsan.bin"
export TSAN_OPTIONS="abort_on_error=0"
setarch $(uname -m) -R ./build/x86_64/debug_tsan/RtspAnalyser/RtspAnalyser -c rtsp_config.json > logs/debug_tsan.log 2>&1

timedatectl
