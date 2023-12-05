# syntax=docker/dockerfile:latest
#
FROM nvidia/cuda:12.3.1-devel-ubuntu22.04

ARG DEBIAN_FRONTEND=noninteractive

# Install Spack package manager
RUN apt-get update && apt-get install -y build-essential ca-certificates coreutils curl environment-modules gfortran git gpg lsb-release python3 python3-distutils python3-venv unzip zip
RUN mkdir -p /spack && curl -sL https://github.com/spack/spack/archive/v0.21.0.tar.gz | tar -xz --strip-components 1 -C /spack

RUN echo "source /spack/share/spack/setup-env.sh" > /etc/profile.d/z09_source_spack_setup.sh

SHELL ["/bin/bash", "-l", "-c"]

RUN spack install geant4
RUN spack install boost+system+program_options+regex+filesystem
RUN spack install cmake
RUN spack install nlohmann-json

RUN apt-get update && apt-get install -y libglm-dev libglfw3-dev libimgui-dev libglew-dev libglu1-mesa-dev

# Strip all the binaries
#RUN find -L /spack/opt/spack -type f -exec readlink -f '{}' \; | xargs file -i | grep 'charset=binary' | grep 'x-executable\|x-archive\|x-sharedlib' | awk -F: '{print $1}' | xargs strip -S

RUN sed -i '/#!\/bin\/bash/c#!\/bin\/bash -l' /opt/nvidia/nvidia_entrypoint.sh
RUN sed -i '/       autoload: direct/c\       autoload: none'  /spack/etc/spack/defaults/modules.yaml
RUN spack module tcl refresh -y
RUN cp -r /spack/share/spack/modules/$(spack arch) /opt/
RUN spack module tcl loads geant4 boost cmake nlohmann-json >> /etc/profile.d/z10_load_spack_modules.sh
RUN echo "export LD_LIBRARY_PATH=/usr/local/lib:$BOOST_ROOT/lib:$LD_LIBRARY_PATH" > /etc/profile.d/z20_docker_init.sh

RUN git clone https://github.com/boost-cmake/bcm.git
RUN cmake -S bcm -B /build/bcm && cmake --build /build/bcm --parallel $(nproc)  && cmake --install /build/bcm

RUN git clone https://github.com/SergiusTheBest/plog.git
RUN cmake -S plog -B /build/plog && cmake --build /build/plog --parallel $(nproc) && cmake --install /build/plog

RUN apt update && apt install -y libssl-dev python-is-python3
RUN apt install -y vim
RUN apt install -y file xorg-dev
RUN apt update && apt install -y libimgui-dev

COPY NVIDIA-OptiX-SDK-7.5.0-linux64-x86_64.sh ./

RUN chmod u+x ./NVIDIA-OptiX-SDK-7.5.0-linux64-x86_64.sh
RUN mkdir -p /NVIDIA-OptiX-SDK-7.5.0-linux64-x86_64
RUN ./NVIDIA-OptiX-SDK-7.5.0-linux64-x86_64.sh --skip-license --prefix=/NVIDIA-OptiX-SDK-7.5.0-linux64-x86_64

WORKDIR /opticks

ENV OPTICKS_PREFIX=/usr/local/opticks
ENV OPTICKS_CUDA_PREFIX=/usr/local/cuda-12.3
ENV OPTICKS_OPTIX_PREFIX=/NVIDIA-OptiX-SDK-7.5.0-linux64-x86_64/
ENV OPTICKS_COMPUTE_CAPABILITY=70
ENV OPTIX_VERSION=70500

COPY cmake /opticks/cmake/

COPY okconf /opticks/okconf/
RUN cmake -S okconf -B /build/okconf -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/okconf --parallel $(nproc)
RUN cmake --install /build/okconf

COPY ana /opticks/ana
COPY sysrap /opticks/sysrap
RUN cmake -S sysrap -B /build/sysrap -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/sysrap --parallel $(nproc)
RUN cmake --install /build/sysrap

COPY boostrap /opticks/boostrap
RUN cmake -S boostrap -B /build/boostrap -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/boostrap --parallel $(nproc)
RUN cmake --install /build/boostrap

COPY npy /opticks/npy
RUN cmake -S npy -B /build/npy -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/npy --parallel $(nproc)
RUN cmake --install /build/npy

COPY optickscore /opticks/optickscore
RUN cmake -S optickscore -B /build/optickscore -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/optickscore --parallel $(nproc)
RUN cmake --install /build/optickscore

COPY CSG /opticks/CSG
RUN cmake -S CSG -B /build/CSG -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/CSG --parallel $(nproc)
RUN cmake --install /build/CSG

COPY qudarap /opticks/qudarap
RUN cmake -S qudarap -B /build/qudarap -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/qudarap --parallel $(nproc)
RUN cmake --install /build/qudarap

COPY CSGOptiX /opticks/CSGOptiX
RUN cmake -S CSGOptiX -B /build/CSGOptiX -DCMAKE_MODULE_PATH=/opticks/cmake/Modules -DCMAKE_BUILD_TYPE=Debug
RUN cmake --build /build/CSGOptiX --parallel $(nproc)
RUN cmake --install /build/CSGOptiX

COPY ggeo /opticks/ggeo
RUN cmake -S ggeo -B /build/ggeo -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/ggeo --parallel $(nproc)
RUN cmake --install /build/ggeo

COPY gdxml /opticks/gdxml
RUN cmake -S gdxml -B /build/gdxml -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/gdxml --parallel $(nproc)
RUN cmake --install /build/gdxml

COPY u4 /opticks/u4
RUN cmake -S u4 -B /build/u4 -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/u4 --parallel $(nproc)
RUN cmake --install /build/u4

COPY g4cx /opticks/g4cx
RUN cmake -S g4cx -B /build/g4cx -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/g4cx --parallel $(nproc)
RUN cmake --install /build/g4cx

COPY opticksgeo /opticks/opticksgeo
RUN cmake -S opticksgeo -B /build/opticksgeo -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/opticksgeo --parallel $(nproc)
RUN cmake --install /build/opticksgeo

COPY extg4 /opticks/extg4
RUN cmake -S extg4 -B /build/extg4 -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/extg4 --parallel $(nproc)
RUN cmake --install /build/extg4

COPY CSG_GGeo /opticks/CSG_GGeo
RUN cmake -S CSG_GGeo -B /build/CSG_GGeo -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
RUN cmake --build /build/CSG_GGeo --parallel $(nproc)
RUN cmake --install /build/CSG_GGeo

# deprecated
#COPY oglrap /opticks/oglrap
#RUN cmake -S oglrap -B /build/oglrap -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
#RUN cmake --build /build/oglrap --parallel $(nproc)
#RUN cmake --install /build/oglrap

#COPY opticksgl /opticks/opticksgl
#RUN cmake -S opticksgl -B /build/opticksgl -DCMAKE_MODULE_PATH=/opticks/cmake/Modules
#RUN cmake --build /build/opticksgl --parallel $(nproc)
#RUN cmake --install /build/opticksgl
