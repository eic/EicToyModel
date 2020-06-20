#
# . <root-installation-directory>/bin/thisroot.sh
#
# mkdir build && mkdir build
# cmake -Wno-dev -DCMAKE_INSTALL_PREFIX=. ..
# make 
#
# -DBFIELD=<BeastMagneticField-installation-directry>
#
# . <geant-installation-directory>/bin/geant4.sh
# -DGEANT=YES
#
# -DOPENCASCADE=<OpenCascade-installation-directory> 
#    1) Edit Standard_CString.hxx (in the installation area): Printf() -> _Printf();
#    2) do not forget to append LD_LIBRARY_PATH accordingly;
#

PROJECT(ETM)

# Install under /usr/local per default;
#IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#  SET(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation directory" FORCE)
#ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

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

if(DEFINED GEANT)
  find_package(Geant4 REQUIRED ui_all vis_all)
  include(${Geant4_USE_FILE})
endif()
SET( CMAKE_CXX_FLAGS "-g -Wall -Wno-pedantic" )
if(DEFINED GEANT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_ETM2GEANT_")
endif()
if(DEFINED OPENCASCADE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_OPENCASCADE_")

  INCLUDE_DIRECTORIES ( 
    ${OPENCASCADE}/include/opencascade
  )
endif()
if(DEFINED BFIELD)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_BFIELD_")

  include_directories( ${BFIELD}/include )
endif()

SET(LIBNAME etm)
SET(EXENAME exe)

SET( LIBSRC 
  source/EtmOrphans.cc 
  source/EtmLine2D.cc 
  source/EtmEtaBoundary.cc 
  source/EtmPolygonGroup.cc 
  source/EtmPalette.cc 
  source/EtmVacuumChamber.cc 
  source/EtmAzimuthalScan.cc 
  source/EtmDetector.cc 
  source/EtmDetectorStack.cc 

  source/EicToyModel.cc 

  source/vc2020_03_20.cc 
)

INCLUDE_DIRECTORIES ( 
  ${ROOT_INCLUDE_DIR}

  ${CMAKE_SOURCE_DIR}/include
)

# This will include all other ones; fine;
SET( HEADERS 
  include/EicToyModel.h 
  include/vc2020_03_20.h
)

ROOT_GENERATE_DICTIONARY(G__${LIBNAME} ${HEADERS} LINKDEF include/etmLinkDef.h)
SET( LIBSRC ${LIBSRC} G__${LIBNAME}.cxx )

ADD_LIBRARY( ${LIBNAME} SHARED ${LIBSRC} ${HEADERS} )
TARGET_LINK_LIBRARIES( ${LIBNAME} ${ROOT_LIBRARIES} Geom)
if(DEFINED BFIELD)
  TARGET_LINK_LIBRARIES( ${LIBNAME} ${BFIELD}/lib/libbmf.so)
endif()
if(DEFINED OPENCASCADE)
  TARGET_LINK_LIBRARIES( ${LIBNAME} -L${OPENCASCADE}/lib -lTKSTEP -lTKIGES -lTKXDEIGES -lTKXDESTEP -lTKSTL -lTKPrim)
endif()
if(DEFINED GEANT)
  TARGET_LINK_LIBRARIES( ${LIBNAME} ${Geant4_LIBRARIES} )
endif()

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include/etm PATTERN "e*.h" EXCLUDE)
INSTALL(TARGETS   ${LIBNAME}                   DESTINATION lib)
