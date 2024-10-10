# RTSP Analyser

Analyse video stream to detect human.

Make lightweight to be execute on RPI 4.

## Workflow

![Workflow](./doc/uml/out/workflow.png)

## Tools to build

- CMake
- Vcpkg
- Ninja

## Dependencies

- OpenCV 4
- GStreamer
- Niels Lohmann JSON

## Installation

### Libs

```bash
sudo apt install libgstreamer-plugins-good1.0-0 libgstreamer-plugins-good1.0-dev gstreamer1.0-plugins-good libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio libgstrtspserver-1.0-dev gstreamer1.0-rtsp libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev


```

# Raspberry Pi OS - Raspbian

```bash
sudo apt update
sudo apt install -y git wget curl build-essential make cmake ninja-build bison
sudo apt install -y bison meson autoconf libtool libx11-dev libxft-dev libxext-dev libxi-dev libxtst-dev libxrandr-dev nasm gcc-11
sudo apt install -y ffmpeg libopencv-dev libopencv-highgui-dev libopencv-objdetect-dev opencv-data


# Vcpkg install https://learn.microsoft.com/fr-fr/vcpkg/get_started/get-started?pivots=shell-bash

git clone https://github.com/microsoft/vcpkg.git

cd vcpkg
export VCPKG_FORCE_SYSTEM_BINARIES=arm
./bootstrap-vcpkg.sh -disableMetrics

# changé param install vcpkg en arm

```

## TO DO

- Test & Impl Mouvement detectection
- Test & Impl Tensorflow Lite human detection via image classification thanks to MobilNetv2
- Impl SMTP client to send e-mail
- Use JSON for config file
- ...
