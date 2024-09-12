FROM --platform=linux/amd64 ubuntu:20.04

ARG USERNAME=meshlab
ARG USER_UID=1000
ARG USER_GID=$USER_UID

ARG QT_VERSION=5.15.2

ENV PATH=/opt/qt/${QT_VERSION}/gcc_64/bin:$PATH
ENV LD_LIBRARY_PATH=/opt/qt/${QT_VERSION}/gcc_64/lib/
ENV QT_PLUGIN_PATH=/opt/qt/${QT_VERSION}/gcc_64/plugins/
ENV QML_IMPORT_PATH=/opt/qt/${QT_VERSION}/gcc_64/qml/
ENV QML2_IMPORT_PATH=/opt/qt/${QT_VERSION}/gcc_64/qml/

RUN apt-get -y update && DEBIAN_FRONTEND=noninteractive apt-get -y install \
    build-essential \
    dbus-x11 \
    fontconfig \
    fuse \
    gdb \
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
    patchelf \
    packagekit-gtk3-module \
    # to be removed
    python3 \
    python3-pip \
    unzip \
    wget

RUN pip3 install aqtinstall

RUN aqt install-qt -O /opt/qt linux desktop ${QT_VERSION}  && \
    pip3 uninstall -y aqtinstall && \
    apt-get -y remove python3-pip python3

# install latest cmake and embree
RUN wget -qO- "https://cmake.org/files/v3.19/cmake-3.19.2-Linux-x86_64.tar.gz" \
    | tar --strip-components=1 -xz -C /usr/local &&\
    wget -qO- "https://github.com/embree/embree/releases/download/v4.3.2/embree-4.3.2.x86_64.linux.tar.gz" \
    | tar -xz -C /usr/local && \
    bash /usr/local/embree-vars.sh  && \
    wget -q "https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-linux.zip" && \
    unzip -o ninja-linux.zip  -d /usr/bin/ && \
    rm ninja-linux.zip && \
    chmod +x /usr/bin/ninja && \
    apt-get -y remove wget unzip && \
    apt-get -y autoremove && \
    apt-get -y clean && \
    rm -rf /var/lib/apt/lists/*

# Generating a universally unique ID for the Container
RUN  dbus-uuidgen > /etc/machine-id

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    # [Optional] Add sudo support.
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

USER $USERNAME