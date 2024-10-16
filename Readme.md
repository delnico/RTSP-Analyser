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
sudo apt install -y git wget curl build-essential make cmake ninja-build pkg-config autoconf automake libtool bison meson
sudo apt install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
sudo apt install -y libx11-dev libxft-dev libxext-dev libxi-dev libxtst-dev libxrandr-dev nasm gcc-11 libgles2-mesa-dev libdbus-1-dev:arm64 libsystemd-dev libglib2.0-dev libatspi2.0-dev
sudo apt install -y ffmpeg libopencv-dev libopencv-highgui-dev libopencv-objdetect-dev opencv-data

sudo pip install --upgrade --break-system-packages meson ninja


# Vcpkg install https://learn.microsoft.com/fr-fr/vcpkg/get_started/get-started?pivots=shell-bash

git clone https://github.com/microsoft/vcpkg.git

cd vcpkg
export VCPKG_FORCE_SYSTEM_BINARIES=1
./bootstrap-vcpkg.sh -disableMetrics
```

Modify **.bashrc**

```bash
export VCPKG_ROOT="/home/pi/vcpkg"

export PATH="$PATH:$VCPKG_ROOT"
export VCPKG_FORCE_SYSTEM_BINARIES=1
```

# Compile to ARM from x86_64 architecture

Thanks to ![www.luisllamas.es](https://www.luisllamas.es/en/how-to-cross-compile-c-for-arm-from-an-x86-x64-computer/)

```bash
sudo apt-get install libc6-armel-cross libc6-dev-armel-cross binutils-arm-linux-gnueabi libncurses5-dev build-essential bison flex libssl-dev bc

# So Raspberry PI 4 work with aarch64 CPU
sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

## TO DO

- Test & Impl Mouvement detectection
- Test & Impl Tensorflow Lite human detection via image classification thanks to MobilNetv2
- Impl SMTP client to send e-mail
- Use JSON for config file
- ...
