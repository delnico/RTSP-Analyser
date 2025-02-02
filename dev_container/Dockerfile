FROM ubuntu:22.04


#   !!!     Security issue      --> no in production
#
#   -   Hard coded passord
#   -   Root acces via SSH permited

RUN useradd -m -s /bin/bash dev && \
    echo "dev:dev" | chpasswd && \
    usermod -aG sudo dev

RUN touch /home/dev/.bashrc
RUN echo 'export DEBIAN_FRONTEND=noninteractive' >> /home/dev/.bashrc

RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get upgrade -y

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y \
    zip unzip tar openssh-server sudo nano \
    git wget curl build-essential g++-11 make cmake ninja-build pkg-config autoconf automake libtool bison meson autoconf-archive python3 python3-pip python3-dev \
    mesa-opencl-icd clinfo vulkan-tools \
    libx11-dev libxft-dev libxext-dev libxi-dev libxtst-dev libxrandr-dev nasm gcc-11 libgles2-mesa-dev libdbus-1-dev libsystemd-dev libglib2.0-dev libatspi2.0-dev \
    libgtk2.0-dev ffmpeg libopencv-dev libopencv-highgui-dev libopencv-objdetect-dev opencv-data opencl-headers libgtk-3-dev \
    libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev libx265-dev libjpeg-dev libpng-dev libtiff-dev \
    gfortran openexr libatlas-base-dev python3-dev python3-numpy libtbb12 libtbb-dev libdc1394-25 libdc1394-dev libopenexr-dev \
    libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev ocl-icd-opencl-dev libvulkan-dev libglew-dev ocl-icd-dev

RUN pip install numpy

# Vcpkg install
RUN git clone https://github.com/microsoft/vcpkg.git
RUN vcpkg/bootstrap-vcpkg.sh -disableMetrics
RUN echo 'export VCPKG_ROOT="/home/$USER/vcpkg"' >> /home/dev/.bashrc && \
    echo 'export PATH="$PATH:$VCPKG_ROOT"' >> /home/dev/.bashrc

# Build nlohmann/json
RUN mkdir -p /home/dev/libs && \
    cd /home/dev/libs && \
    git clone https://github.com/nlohmann/json.git && \
    cd json && \
    mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Build OpenCV with contrib
RUN mkdir -p /home/dev/libs/opencvlib && \
    cd /home/dev/libs/opencvlib && \
    git clone https://github.com/opencv/opencv.git && \
    git clone https://github.com/opencv/opencv_contrib.git && \
    mkdir build && cd build && \
    cmake \
      -D ENABLE_NEON=ON \
      -D WITH_GTK=ON \
      -D WITH_OPENCL=ON \
      -D WITH_VULKAN=ON \
      -D WITH_OPENGL=ON \
      -D WITH_TBB=ON \
      -D BUILD_EXAMPLES=OFF \
      -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules \
      ../opencv && \
    cmake --build . -j$(nproc) && \
    make install && \
    ldconfig


# Build TensorflowCC
RUN mkdir -p /home/dev/libs && \
    cd /home/dev/libs && \
    git clone https://github.com/FloopCZ/tensorflow_cc.git && \
    cd tensorflow_cc/tensorflow_cc && \
    mkdir build && cd build && \
    export CC=/usr/bin/gcc-11 && \
    export CXX=/usr/bin/g++-11 && \
    export CC_OPT_FLAGS="-march=native" && \
    cmake -DLOCAL_RAM_RESOURCES=2048 -DLOCAL_CPU_RESOURCES=1 .. && \
    make && \
    make install && \
    ldconfig

RUN mkdir /var/run/sshd && \
    echo "PermitRootLogin yes" >> /etc/ssh/sshd_config && \
    echo "PasswordAuthentication yes" >> /etc/ssh/sshd_config


RUN apt-get install -y x11-apps

RUN chmod 755 /tmp/.X11-unix

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
