FROM ubuntu:24.04
# Dependencies from central repo
RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y tzdata
RUN apt-get update && apt-get install -y clang-19 cmake git python3-pip pkg-config curl ninja-build zip \
    libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
    xorg-dev


# Set compiler
ENV CC=clang-19
ENV CXX=clang++-19

# Install VCPKG
RUN git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg --depth=1 && cd /opt/vcpkg/ && ./bootstrap-vcpkg.sh
ENV VCPKG_ROOT=/opt/vcpkg

# Entry
ENV HOME=/root/
ENV USER=root
ENTRYPOINT /bin/bash