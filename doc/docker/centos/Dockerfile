#
# See README.md for building and running instructions;
#

FROM centos:7

# Install the missing packages;
RUN yum -y install gcc-c++ gcc-gfortran curl curl-devel nano git make libXft-devel libXpm-devel giflib libtiff && \
    yum -y clean all

# Install ROOT; let it be 6.14.04;
ARG ROOT=root_v6.14.04.Linux-centos7-x86_64-gcc4.8.tar.gz

RUN cd /opt && \
  curl -O https://root.cern/download/${ROOT} && \
  tar xfz ${ROOT} && \
  rm -f ${ROOT}

# Centos 7 is shipped with cmake 2.8.12; need a newer one;
ARG VCMAKE=3.17.1
RUN cd /tmp && curl -LO https://github.com/Kitware/CMake/releases/download/v${VCMAKE}/cmake-${VCMAKE}.tar.gz && \
    tar xfz cmake-${VCMAKE}.tar.gz && \
    cd cmake-${VCMAKE} && \
    ./bootstrap --system-curl --prefix=/usr/local && \
    make -j4 install && \
    cd .. && rm -rf cmake-${VCMAKE}*

# Also git 1.8.3 gives 'unable to look up current user in the passwd file' on Linux; install a newer one;
ARG GIT=git-2.25.4
RUN cd /tmp && curl -O https://mirrors.edge.kernel.org/pub/software/scm/git/${GIT}.tar.gz && \
    tar xfz ${GIT}.tar.gz && \ 
    cd ${GIT} && \
    ./configure --prefix=/usr/local && \
    make -j4 install && \
    cd .. && rm -rf ${GIT}*
