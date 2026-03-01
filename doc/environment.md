# Environment

## Compiler, libs, ...

```bash
sudo apt install -y \
  mesa-opencl-icd clinfo vulkan-tools \
  git wget curl build-essential make cmake ninja-build pkg-config autoconf automake libtool bison meson autoconf-archive libc++-dev \
  libx11-dev libxft-dev libxext-dev libxi-dev libxtst-dev libxrandr-dev nasm gcc-11 libgles2-mesa-dev libdbus-1-dev libsystemd-dev libglib2.0-dev libatspi2.0-dev \
  libgtk2.0-dev ffmpeg libopencv-dev libopencv-highgui-dev libopencv-objdetect-dev opencv-data opencl-headers libgtk-3-dev \
  libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev libx265-dev libjpeg-dev libpng-dev libtiff-dev \
  gfortran openexr libatlas-base-dev python3 python3-pip python3-dev python3-numpy libtbb12 libtbb-dev libdc1394-25 libdc1394-dev libopenexr-dev \
  libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev ocl-icd-opencl-dev libvulkan-dev libglew-dev ocl-icd-dev flex

pip install --break-system-packages numpy jinja2
```

## Vcpkg

[Microsoft Guide Vcpkg installation](https://learn.microsoft.com/fr-fr/vcpkg/get_started/get-started?pivots=shell-bash)

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh -disableMetrics
```

### Add to **~/.bashrc**

```bash
export VCPKG_ROOT="/home/$USER/vcpkg"
export PATH="$PATH:$VCPKG_ROOT"
```
