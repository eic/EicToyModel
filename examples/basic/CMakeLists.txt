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

# Find ROOT;
FIND_PACKAGE(ROOT 6.0.0 REQUIRED COMPONENTS RIO Tree Hist)
#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

SET( CMAKE_CXX_FLAGS "-g -Wall -Wno-pedantic" )

SET(EXENAME basic)

SET( EXESRC main.cc )

INCLUDE_DIRECTORIES ( 
  ${ROOT_INCLUDE_DIR}

  ${ETM}/include/etm
)

ADD_EXECUTABLE( ${EXENAME} ${EXESRC} )
TARGET_LINK_LIBRARIES( ${EXENAME} ${ROOT_LIBRARIES} ${Geant4_LIBRARIES} -L${ETM}/lib etm )
