#
# . <root-installation-directory>/bin/thisroot.sh
# . <geant-installation-directory>/bin/geant4.sh
#
# export LD_LIBRARY_PATH=<EicToyModel-installation-directory>/lib:<VGM-library-location>:${LD_LIBRARY_PATH}
#
# mkdir build && mkdir build
# cmake -Wno-dev -DETM=<EicToyModel-installation-directory> ..
# make 
#
# ./eicroot <path-to-EicToyModel-root-file> <path-to-media.geo-file>
#

PROJECT(EICROOT)

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

SET(EXENAME eicroot)

SET( EXESRC main.cc )

INCLUDE_DIRECTORIES ( 
  ${ROOT_INCLUDE_DIR}

  ${ETM}/include/etm
)

ADD_EXECUTABLE( ${EXENAME} ${EXESRC} )
TARGET_LINK_LIBRARIES( ${EXENAME} ${ROOT_LIBRARIES} Geom ${Geant4_LIBRARIES} -L${ETM}/lib etm )
