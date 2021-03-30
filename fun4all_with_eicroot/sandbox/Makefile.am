AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include \
  -I$(G4_MAIN)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  EicToyModelSubsystem.h \
  EicRootSubsystem.h \
  EicRootVstSubsystem.h \
  EicRootMuMegasSubsystem.h \
  EicRootGemSubsystem.h \
  TrackFastSimEval.h

lib_LTLIBRARIES = \
  libeicdetectors.la

libeicdetectors_la_SOURCES = \
  EicToyModelSubsystem.cc \
  EicToyModelDetector.cc \
  EicRootSubsystem.cc \
  EicRootDetector.cc \
  EicRootSteppingAction.cc \
  TrackFastSimEval.cc

libeicdetectors_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lg4detectors \
  -lg4testbench \
  -ltrackbase_historic_io \
  -letm \
  -lBaseVGM \
  -lGeant4GM \
  -lRootGM \
  -lClhepVGM

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libeicdetectors.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
