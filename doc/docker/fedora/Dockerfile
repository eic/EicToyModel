#
# See README.md for building and running instructions;
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
