FROM ubuntu

ARG DEBIAN_FRONTEND=noninteractive

# nvidia docker runtime env
ENV NVIDIA_VISIBLE_DEVICES \
        ${NVIDIA_VISIBLE_DEVICES:-all}
ENV NVIDIA_DRIVER_CAPABILITIES \
        ${NVIDIA_DRIVER_CAPABILITIES:+$NVIDIA_DRIVER_CAPABILITIES,}graphics,compat32,utility

RUN apt-get update &&\
    apt-get install -y \
        nano git git-lfs \
        build-essential \
        gdb \
        cmake \
        python3 python3-pip \
        libglfw3-dev \
        xorg-dev \
        x11-apps

ARG EMSDK_TAG=main
WORKDIR /usr/local/src/emsdk
RUN cd /usr/local/src &&\
    git clone https://github.com/emscripten-core/emsdk.git &&\
    cd emsdk &&\
    git checkout ${EMSDK_TAG} &&\
    ./emsdk install latest &&\
    ./emsdk activate latest &&\
    chmod +x emsdk_env.sh
RUN echo 'source "/usr/local/src/emsdk/emsdk_env.sh"' >> $HOME/.bash_profile
ENV EMSDK_ROOT="/usr/local/src/emsdk"

ARG IMGUI_TAG=v1.89
WORKDIR /usr/local/src/imgui
RUN cd /usr/local/src/ &&\
    git clone https://github.com/ocornut/imgui.git &&\
    cd imgui &&\
    git checkout ${IMGUI_TAG}
ENV IMGUI_ROOT="/usr/local/src/imgui"

ARG IMNODES_TAG=v0.5
WORKDIR /usr/local/src/imnodes
RUN cd /usr/local/src &&\
    git clone https://github.com/Nelarius/imnodes &&\
    cd imnodes &&\
    git checkout ${IMNODES_TAG}
ENV IMNODES_ROOT="/usr/local/src/imnodes"

ARG JSON_TAG=v3.11.2
WORKDIR /usr/local/src/json
RUN cd /usr/local/src &&\
    git clone https://github.com/nlohmann/json.git &&\
    cd json &&\
    git checkout ${JSON_TAG}
ENV JSON_ROOT="/usr/local/src/json"