#
# Supposed to work under Linux and Mac OS with docker and X11 installed;
#
# Create scratch directory (if want to preserve session results); 
#   mkdir <my-scratch-dir>
#
# Linux:
#   sudo docker build -t fedora-eictoymodel .
#   sudo docker run --rm -it -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY --user $(id -u) -v <my-scratch-dir>:/scratch fedora-eictoymodel
#
# Mac OS:   1) XQuartz: Preferences -> Security -> enable "Allow connections from network clients"; restart XQuartz;
#           2) execute 'xhost +localhost' in a terminal;
#        docker build -t fedora-eictoymodel .
#        docker run --rm -it -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=host.docker.internal:0   -v <my-scratch-dir>:/scratch fedora-eictoymodel
#
# . /opt/root/bin/thisroot.sh
# cd /scratch 
# git clone https://github.com/eic/EicToyModel.git
# cd EicToyModel && mkdir build && cd build
# cmake ..
# make -j4
# root -l ../scripts/example.C
#  

FROM fedora:30

# NB: user is ROOT here; now install the missing packages;
RUN yum -y install gcc-c++ gcc-gfortran curl nano git cmake make libXft-devel libXpm-devel giflib libtiff && \
    yum -y clean all

# Install ROOT; let it be 6.18.04;
ARG ROOT=root_v6.18.04.Linux-fedora30-x86_64-gcc9.2.tar.gz

RUN cd /opt && \
  curl -O https://root.cern/download/${ROOT} && \
  tar xfz ${ROOT} && \
  rm -f ${ROOT}
