
  EicToyModel is a C++ ROOT-based software suite for EIC Central Detector 
configuration.  

  The main objective of this software was providing the EIC community users with a tool, which would allow them to easily (whatever easy means) create possible EIC detector configuration *templates* (namely, the self-consistent collections of 3D sub-detector intergation volumes) and make use of them in the GEANT simulation environment.

  The integration volumes are guaranteed to not overlap either with each other or with the IR vacuum chamber volume within the same EIC detector configuration.

  They move synchronously under generic changes to the IR layout (e.g. by the nominal IP shift along the beam line direction and/or by a change in the acceptance boundaries separating barrel and endap regions).

  Technically a user has access to the parameters characterizing the shape and location of the integration volume to where his/her sub-detector component belongs, and can build the respective sub-detector dynamically in the respective GEANT G4VUserDetectorConstruction::Construct() method depending e.g. on the sub-detector distance from the nominal IP.


Other features
--------------

* On-disk model representation (C++ class instance export/import using ROOT serializer).
* Dynamic generation of the GEANT representation of the detector integration volumes.
* Parameteric description of the IR vacuum chamber volumes, representing reasonably 
well the available 25mrad crossing angle configuration as of 2020/03/20. As such it 
should be suitable to the second IR vacuum chamber description. 
* Configurable vacuum chamber geometry export in the GDML and ROOT TGeo formats.
* Interface to the BeastMagneticField library (BeAST field map import, as well as
import of the soon-to-become-available magnetic field maps produced by the EIC 
greenfield solenoid design team).
* Tools for the magnetic field B*dl integral and the IR vacuum chamber material scans.
* Export of the integration volumes as a STEP file. 


Model limitations
-----------------

* The EIC Central Detector is assumed to be composed of exactly four non-overlapping
regions (vertex, barrel, and two endcaps).
* Sub-detector integration volumes in a 2D view of a given region are flat
objects, stacked along the electron beam line for the two endcaps and the normal to 
the electron beam axis for the vertex and barrel stacks. See the picture below. 
* Far-forward stack (and B0 magnet in particular) can probably be added with minor
complications, but is not considered in the present implementation.
* All stacks are organized in a "projective" geometry, which should represent a typical 
layout of a 4pi EIC Central Detector well enough at this stage. In this sense the only 
reason why a separate vertex stack exists is that its "silicon tracker volume" can 
occupy a physical area wider than a "TPC volume" of a barrel stack.
* A "crack" between the barrel and the endcap stacks in a 2D {Z,R} view is represented by
monotonous functions in both R(Z) and Z(R) representations. "Crack" shape however can 
be configured in a rather flexible way, see examples below.

   

Pre-requisites
--------------

A library has to be installed locally. 

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
# if GEANT4 interface is required:
#   -DGEANT=YES
# for CAD export:
#   -DOPENCASCADE=<OpenCascade-installation-directory>
# for magnetic field map interface:
#   -DBFIELD=<BeastMagneticField-installation-directry>

make 
```

Running
-------
  
Edit a script like ![example.C](scripts/example.C) in the ../scripts/ directory 
according to your preferences (see full list of the available commands 
![here](doc/README.API.md)) and runs it:

```
root -l ../scripts/example.C
```

A ROOT canvas will appear with a picture similar to the image below.

![](doc/sandbox.root.png)

A limited set of interactive commands (see full list here) is available. Try e.g. the 
following sequence with the safe-explaining results: 

```
root [] eic->width(1200); 
root [] eic->mirror();
root [] eic->mirror(false);
root [] eic->ip(0);
root [] eic->ip(-50 * etm::cm);
# Vertical and horizontal plane cross cut; in the latter case the 25mrad crossing 
# angle is taken into account; 
root [] eic->vdraw();
root [] eic->hdraw();
# Remove e/m calorimeter from the backward (electron-going) endcap;
root [] eic->bck()->rm("EmCal");
# Install a 20cm long MPGD "detector" in front of the high-momentum RICH in 
# the hadron-going endcap;
root [] eic->fwd()->insert("MPGD", 20 * etm::cm, "HM RICH");
# Save the current configuration;
root [] eic->write();
# Save example.vc.gdml file with the vacuum chamber layout; a BUG: command 
# works properly only once; 
root [] eic->ExportVacuumChamber();
root [] .q
```

Re-open the produced file with the model in ROOT if needed:

```
root -l '../scripts/reader.C("example.root")'
```

Beyond this point one can work with the model the same way as if it was created 
from scratch (see example.C above); it is strongly recommended to use detector 
composition changing commands like rm() and insert() only as a quick tuning 
means, and once a desired composition is found, put the respective changes
int a full script, creating a given model from scratch; the reason is simple:
this software is in the early debugging stage, and it is much more likely 
than a file format will change rather than the API of the commands will change 
(and the latter can be fixed by hand if needed);

```
root [] auto eic = EicToyModel::Instance(); eic->hdraw();
```

It should be noted that if the geometry was saved using eic->write(true) call (notice
'true' argument), a naive permanent lock is applied to the contents of the binary 
ROOT file. The geometry can not be modified any longer without hacking the library
(would be a very easy task for a junior C++ programmer, admittedly).


GEANT interface
---------------

Apart from the eic->ExportVacuumChamber() command shown above one can create G4 
detector integration volumes on the fly, either one at a time or all of them at 
once. See a short example executable ![main.cc](source/main.cc) as an example. 

The library should be configured with the -DGEANT cmake command line key (see above).

The integration volumes are currently represented as G4GenericPolycone shapes.
G4BREPSolidPolyhedra option will follow soon. The interface producing an asymmetric
boolean cut by the vacuum chamber shape is in the debugging stage now.

Naming convention for these volumes may require some tuning.

Once a user gets access to a particular logical volume, he/she is free to populate 
this volume with the daughter objects, observing the usual GEANT boundary conditions.
Volumes in the endcaps are shifted (a BUG: not yet!) to their geometric center along
the beam line. Volumes in the endcaps will be shifted towards the IP. This way, to 
first order, whatever objects are placed inside the integration volumes, the geometry 
will be consistent after moderate re-shuffling of a particular detector stack (say, 
after removing one of the TRD volumes in the hadron-going endcap the e/m calorimeter
behind it will be located in a proper place). 

It seems to be wise to check the integration volume actual location by means of the 
available library calls, and tune the detector geometry accordingly if needed.

Apparently the community may want to decide exporting *individual* GDML objects
describing the integration volumes for a given version of the full EIC detector
geometry, and avoid the dependency on either ETM (EIC Toy Model) library described 
here or on ROOT in the GEANT environment alltogether. Providing consistency between 
different subdetector systems may be problematic in this case though.

```
# The following command brings up the G4 Qt window with the imported model:
./exe example.root
```

CAD interface
-------------


Magnetic field interface
------------------------


B*dl integral and vacuum chamber material scans
----------------------------------------------- 


ROOT macro options
------------------

Full current list of the options is available ![here](doc/README.API.md).