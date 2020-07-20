#
# See README.md for building and running instructions;
#

FROM ubuntu:eoan

# Install the missing packages;
RUN apt-get update && \
  apt-get -y install curl nano git cmake make libxft-dev libxpm-dev libtiff5 && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/*

# Install ROOT; let it be 6.20.04;
ARG ROOT=root_v6.20.04.Linux-ubuntu19-x86_64-gcc9.2.tar.gz

RUN cd /opt && \
  curl -O https://root.cern/download/${ROOT} && \
  tar xfz ${ROOT} && \
  rm -f ${ROOT}
