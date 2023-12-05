# syntax=docker/dockerfile:latest

FROM nvcr.io/nvidia/cuda:11.8.0-devel-ubuntu22.04

ARG DEBIAN_FRONTEND=noninteractive

# Install Spack package manager
RUN apt update && apt install -y build-essential ca-certificates coreutils curl environment-modules gfortran git gpg lsb-release python3 python3-distutils python3-venv unzip zip
RUN mkdir -p /spack && curl -sL https://github.com/spack/spack/archive/v0.21.0.tar.gz | tar -xz --strip-components 1 -C /spack

RUN echo "source /spack/share/spack/setup-env.sh" > /etc/profile.d/z09_source_spack_setup.sh

SHELL ["/bin/bash", "-l", "-c"]

RUN <<EOF
    spack install geant4
    spack install boost+system+program_options+regex+filesystem
    spack install cmake
    spack install nlohmann-json
    spack clean -a
EOF

RUN apt update && apt install -y libglm-dev libglfw3-dev libimgui-dev libglew-dev libglu1-mesa-dev

# Strip all the binaries
#RUN find -L /spack/opt/spack -type f -exec readlink -f '{}' \; | xargs file -i | grep 'charset=binary' | grep 'x-executable\|x-archive\|x-sharedlib' | awk -F: '{print $1}' | xargs strip -S

RUN sed -i '/#!\/bin\/bash/c#!\/bin\/bash -l' /opt/nvidia/nvidia_entrypoint.sh
RUN sed -i '/       autoload: direct/c\       autoload: none'  /spack/etc/spack/defaults/modules.yaml
RUN spack module tcl refresh -y
RUN cp -r /spack/share/spack/modules/$(spack arch) /opt/
RUN spack module tcl loads geant4 clhep boost cmake nlohmann-json >> /etc/profile.d/z10_load_spack_modules.sh

RUN git clone https://github.com/boost-cmake/bcm.git
RUN cmake -S bcm -B /build/bcm && cmake --build /build/bcm --parallel $(nproc)  && cmake --install /build/bcm

RUN git clone https://github.com/SergiusTheBest/plog.git
RUN cmake -S plog -B /build/plog && cmake --build /build/plog --parallel $(nproc) && cmake --install /build/plog

RUN apt update && apt install -y libssl-dev python-is-python3
RUN apt install -y vim

COPY NVIDIA-OptiX-SDK-7.6.0-linux64-x86_64.sh ./

RUN chmod u+x ./NVIDIA-OptiX-SDK-7.6.0-linux64-x86_64.sh
RUN mkdir -p /usr/local/optix
RUN ./NVIDIA-OptiX-SDK-7.6.0-linux64-x86_64.sh --skip-license --prefix=/usr/local/optix

ENV HOME=/root
ENV OPTICKS_HOME=$HOME/opticks
ENV OPTICKS_PREFIX=/usr/local/opticks
ENV OPTICKS_CUDA_PREFIX=/usr/local/cuda
ENV OPTICKS_OPTIX_PREFIX=/usr/local/optix
ENV OPTICKS_COMPUTE_CAPABILITY=35

WORKDIR $OPTICKS_HOME

COPY . $OPTICKS_HOME/
COPY example.opticks_config $HOME/.opticks_config

RUN <<EOF
    source $OPTICKS_HOME/opticks.bash

    opticks-prepend-prefix $(spack find --format "{prefix}" clhep)
    opticks-prepend-prefix $(spack find --format "{prefix}" xerces-c)
    opticks-prepend-prefix $(spack find --format "{prefix}" geant4)
    opticks-prepend-prefix $(spack find --format "{prefix}" boost)

    export PYTHONPATH=$(dirname $OPTICKS_HOME)

    opticks-
    opticks-full
EOF

RUN echo "source ~/.opticks_config" >> $HOME/.bashrc

RUN mkdir -p ~/.opticks/rngcache/RNG

#RUN sed -i '/opticks-prepend-prefix/d' /root/.opticks_config
#RUN echo "opticks-prepend-prefix $(spack find --format "{prefix}" clhep)" >> /root/.opticks_config
#RUN echo "opticks-prepend-prefix $(spack find --format "{prefix}" xerces-c)" >> /root/.opticks_config
#RUN echo "opticks-prepend-prefix $(spack find --format "{prefix}" geant4)" >> /root/.opticks_config
#RUN echo "opticks-prepend-prefix $(spack find --format "{prefix}" boost)" >> /root/.opticks_config
