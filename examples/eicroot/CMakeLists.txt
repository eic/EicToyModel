#
# . <root-installation-directory>/bin/thisroot.sh
# . <geant-installation-directory>/bin/geant4.sh
#
# export LD_LIBRARY_PATH=<EicToyModel-installation-directory>/lib:${LD_LIBRARY_PATH}
#
# mkdir build && mkdir build
# cmake -Wno-dev -DETM=<EicToyModel-installation-directory> ..
# make 
#
# ./basic <path-to-EicToyModel-root-file>
#

PROJECT(BASIC)

# Yes, help my buggy assert() statements; FIXME: do it better later;
IF( NOT CMAKE_BUILD_TYPE )
   SET( CMAKE_BUILD_TYPE Debug)
ENDIF()

# Should suffice?;
cmake_minimum_required(VERSION 2.8.10)

# Assume ROOTSYS was set up in the environment;
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

# Find ROOT;
FIND_PACKAGE(ROOT 6.0.0 REQUIRED COMPONENTS RIO Tree Hist)
#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

SET( CMAKE_CXX_FLAGS "-g -Wall -Wno-pedantic -Wno-reorder" )

SET(EXENAME basic)

SET( EXESRC main.cc )

INCLUDE_DIRECTORIES ( 
  ${ROOT_INCLUDE_DIR}

  ${ETM}/include/etm

  /sl6_home/SL7.1/include
  #/sl6_home/SL7.1/YR/EicRoot/geobase
  #/sl6_home/SL7.1/YR/EicRoot/eic/base
)

#SET( EICROOT /sl6_home/SL7.1/YR/EicRoot/build )

ADD_EXECUTABLE( ${EXENAME} ${EXESRC} )
TARGET_LINK_LIBRARIES( ${EXENAME} ${ROOT_LIBRARIES} Geom ${Geant4_LIBRARIES} -L${ETM}/lib etm -L/sl6_home/SL7.1/lib64 -lRootGM -lGeant4GM -lBaseVGM -lClhepVGM)
