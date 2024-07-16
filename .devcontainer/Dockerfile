FROM ubuntu:20.04

RUN apt-get -y update && DEBIAN_FRONTEND=noninteractive apt-get -y install \
    build-essential \
    dbus-x11 \
    fontconfig \
    fuse \
    git \
    kmod \
    libboost-dev \
    libcanberra-gtk3-0 \
    libdbus-1-3 \
    libegl-dev \
    libfuse2 \
    libgmp-dev \
    libglu1-mesa-dev \
    libmpfr-dev \
    libpulse-mainloop-glib0 \
    libtbb-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-keysyms1-dev \
    libxcb-render-util0-dev \
    libxcb-shape0 \
    libxcb-xinerama0-dev \
    libxerces-c-dev \
    libxkbcommon-x11-dev \
    ninja-build \
    patchelf \
    packagekit-gtk3-module \
    python3 \
    python3-pip

RUN pip3 install aqtinstall

ARG QT=5.15.2
ARG QT_HOST=linux
ARG QT_TARGET=desktop
RUN aqt install-qt -O /opt/qt ${QT_HOST} ${QT_TARGET} ${QT} 

ENV PATH=/opt/qt/${QT}/gcc_64/bin:$PATH
ENV LD_LIBRARY_PATH=/opt/qt/${QT}/gcc_64/lib/
ENV QT_PLUGIN_PATH=/opt/qt/${QT}/gcc_64/plugins/
ENV QML_IMPORT_PATH=/opt/qt/${QT}/gcc_64/qml/
ENV QML2_IMPORT_PATH=/opt/qt/${QT}/gcc_64/qml/

# install latest cmake and embree
RUN apt purge --auto-remove cmake && \
    apt-get -y install wget && \
    wget -qO- "https://cmake.org/files/v3.19/cmake-3.19.2-Linux-x86_64.tar.gz" \
    | tar --strip-components=1 -xz -C /usr/local &&\
    wget -qO- "https://github.com/embree/embree/releases/download/v4.3.2/embree-4.3.2.x86_64.linux.tar.gz" \
    | tar -xz -C /usr/local && \
    bash /usr/local/embree-vars.sh  && \
    apt-get -y remove wget && \
    apt-get -y autoremove && \
    apt-get -y clean && \
    rm -rf /var/lib/apt/lists/*

# Generating a universally unique ID for the Container
RUN  dbus-uuidgen > /etc/machine-id