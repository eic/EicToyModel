
  EicToyModel is a C++ ROOT-based software suite for EIC Central Detector 
configuration. A library has to be installed locally.  

Pre-requisites
--------------

It is assumed that a more or less modern ROOT 6 version is installed and configured 
on the local system. 6.14.00 works. The line below is for bash shell. Replace .sh
by .csh if your shell is csh.

```
. <root-installation-directory>/bin/thisroot.sh
```

If GEANT part of functionality is required, GEANT has to be installed and configured
on the local system. 10.05.p01 works. The line below is for bash shell. Replace .sh
by .csh if your shell is csh.


```
. geant4.10.05.p01.build/bin/geant4.sh
```

Downloading
-----------

```
git clone https://github.com/eic/EicToyModel.git
```

Compiling
---------

```
cd EicToyModel && mkdir build && cd build
cmake -Wno-dev ..

# additional cmake options:
#
# if GEANT4 is required:
#   -DGEANT=YES
# for CAD export:
#   -DOPENCASCADE=<OpenCascade-installation-directory>
# for magnetic field map interface:
#   -DBFIELD=<BeastMagneticField-installation-directry>

make 
```

Running
-------
  
```
# Edit a script like sandbox.C in the *../scripts/* directory according to your 
# preferences (see full list of the available commands here) and runs it:
root -l ../scripts/example.C

# A ROOT canvas will appear with a picture similar to the image below.
```

![](doc/sandbox.root.png)

```
# A limited set of interactive commands (see full list here) is available. Try 
# e.g. the following sequence of commands with the safe-explaining results: 
root [] eic->width(1200); 
root [] eic->mirror();
root [] eic->mirror(off);
root [] eic->ip(0);
root [] eic->mirror(off);
root [] eic->ip(-50 * etm::cm);
# Vertical and horizontal plane cross cut; in the latter case the 25mrad crossing 
# angle is taken into account; 
root [] eic->vdraw();
root [] eic->hdraw();
root [] eic->bck()->rm("EmCal");
root [] eic->fwd()->insert("MPGD", 20 * etm::cm, "HM RICH");
# Save the current configuration;
root [] eic->write();
root [] .q
```

```
# Re-open the produced file with the model in ROOT if needed:
root -l '../scripts/reader.C("example.root")'

# Beyond this point one can work with the model the same way as if it was created 
# from scratch (see example.C above); it is strongly recommended to use detector 
# composition changing commands like rm() and insert() only as a quick tuning 
# means, and once a desired composition is found, put the respective changes
# int a full script, creating a given model from scratch; the reason is simple:
# this software is in the early debugging stage, and it is much more likely 
# than a file format will change rather than the API of the commands will change 
# (and the latter can be fixed by hand if needed);
root [] auto eic = EicToyModel::Instance(); eic->hdraw();
```


```
# GEANT example executable (see main.cc):
./exe sandbox.root
```

ROOT macro options
------------------

Description will follow soon.