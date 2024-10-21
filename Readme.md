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
- Niels Lohmann JSON
- Boost
- Boost Asio

## Raspberry Pi OS - Raspbian

```bash
sudo apt update
sudo apt-get install ocl-icd-opencl-dev
sudo apt install -y git wget curl build-essential make cmake ninja-build pkg-config autoconf automake libtool bison meson autoconf-archive
sudo apt install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
sudo apt install -y libx11-dev libxft-dev libxext-dev libxi-dev libxtst-dev libxrandr-dev nasm gcc-11 libgles2-mesa-dev libdbus-1-dev:arm64 libsystemd-dev libglib2.0-dev libatspi2.0-dev libgtk2.0-dev
sudo apt install -y ffmpeg libopencv-dev libopencv-highgui-dev libopencv-objdetect-dev opencv-data

sudo apt install ocl-icd-opencl-dev ocl-icd-dev opencl-headers clinfo libraspberrypi-dev
sudo apt-get install clang clang-format clang-tidy

sudo pip install --upgrade --break-system-packages meson ninja
```

```bash
# from https://yunusmuhammad007.medium.com/build-and-install-opencv-4-5-3-on-raspberry-pi-3-with-opencl-opengl-and-gstreamer-enable-8c493fc992f0

sudo apt-get install -y \
        build-essential \
        cmake \
        git \
        gfortran \
        libatlas-base-dev \
        libavcodec-dev \
        libavformat-dev \
        libavresample-dev \
        libcanberra-gtk3-module \
        libeigen3-dev \
        libglew-dev \
        libgstreamer-plugins-base1.0-dev \
        libgstreamer-plugins-bad1.0-dev \
        libgstreamer1.0-dev \
        gstreamer1.0-plugins-ugly \
        gstreamer1.0-tools \
        gstreamer1.0-gl \
        libgtk-3-dev \
        libjpeg62-turbo-dev \
        libjpeg-dev \
        libturbojpeg0-dev \
        liblapack-dev \
        liblapacke-dev \
        libopenblas-dev \
        libpng-dev \
        libpostproc-dev \
        libswscale-dev \
        libtbb-dev \
        libtesseract-dev \
        libtiff-dev \
        libv4l-dev \
        libxine2-dev \
        libxvidcore-dev \
        libx264-dev \
        pkg-config \
        python3-dev \
        python3-numpy \
        python3-matplotlib \
        qv4l2 \
        v4l-utils \
        zlib1g-dev
```

## OpenCL driver

Thanks to [https://qengineering.eu/install-opencl-on-raspberry-pi-3.html](https://qengineering.eu/install-opencl-on-raspberry-pi-3.html)

```bash
mkdir -p ~/opencl
cd ~/opencl
git clone https://github.com/doe300/VC4CLStdLib.git
git clone https://github.com/doe300/VC4CL.git
git clone https://github.com/doe300/VC4C.git

# first VC4CLStdLib
cd ~/opencl/VC4CLStdLib
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
sudo ldconfig

# next VC4C
cd ~/opencl/VC4C
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
sudo ldconfig

# last VC4CL
cd ~/opencl/VC4CL
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
sudo ldconfig

# check with clinfo after reboot
clinfo
```

## Vcpkg

```bash
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

## Install dependencies

### nlohmann json

```bash
git clone https://github.com/nlohmann/json.git
cd json
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
```

### OpenCV with contrib

```bash
mkdir opencvlib
cd opencvlib
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
mkdir build
cd build
#cmake -D ENABLE_NEON=ON -DWITH_OPENCL=ON -DWITH_OPENCL_D3D11_NV=OFF -DWITH_OPENCL_SVM=OFF -D WITH_GTK=ON -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules ../opencv
cmake -D ENABLE_NEON=ON -D WITH_GTK=ON -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules ../opencv
cmake --build . -j $(nproc)
sudo make install
```

## TO DO

- Test & Impl Mouvement detectection                Testing
- Test & Impl Tensorflow Lite human detection  
  via image classification thanks to MobilNetv2
- Impl SMTP client to send e-mail
- Use JSON for config file                          OK
- ...
