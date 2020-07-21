
EIC Toy Model (ETM)
=================== 

  EicToyModel (ETM) is a C++ ROOT-based software suite for EIC Central Detector 
configuration purposes.  

 Content:

 * [Introduction](#introduction)
 * [Other features](#other-features)
 * [Model limitations](#model-limitations)
 * [Quick start and basic ROOT functionality](#quick-start-and-basic-functionality)
 * &nbsp; &nbsp; &nbsp; [Mac OS](#mac-os)
 * &nbsp; &nbsp; &nbsp; [Linux](#linux)
 * &nbsp; &nbsp; &nbsp; [Docker environment](#docker-environment)
 * &nbsp; &nbsp; &nbsp; [Vacuum chamber material and B*dl integral scans](#vacuum-chamber-material-and-magnetic-field-scans)
 * [GEANT interface](#geant-interface)
 * &nbsp; &nbsp; &nbsp; [*escalate* Docker container](#escalate-docker-container)
 * &nbsp; &nbsp; &nbsp; [*fun4all* singularity container](#fun4all-singularity-container)
 * [Extended functionaly](#extended-functionality)
 * &nbsp; &nbsp; &nbsp; [CAD interface](#cad-interface)
 * &nbsp; &nbsp; &nbsp; [Magnetic field interface](#magnetic-field-interface)
 * [ROOT macros options](#root-macro-options)

<br/>

Introduction
------------

  The main objective of this software was providing the EIC community users
with a tool, which would allow them to easily (whatever easy means) create possible 
EIC detector configuration *templates* (namely, the self-consistent collections of 
3D sub-detector intergation volumes) and make use of them in the GEANT simulation 
environment.

  The integration volumes are guaranteed to not overlap either with each other
or with the IR vacuum chamber volume within the same EIC detector configuration.

  They move synchronously under generic changes to the IR layout (e.g. by the 
nominal IP shift along the beam line direction and/or by a change in the acceptance
boundaries separating barrel and endap regions).

  Technically, a user has access to the parameters characterizing the shape and 
location of the integration volume to where his/her sub-detector component belongs, 
and can build the sub-detector dynamically in the respective GEANT
G4VUserDetectorConstruction::Construct() method, depending e.g. on the sub-detector 
distance from the nominal IP (*partly implemented*).

  It is assumed that the basic part of functionality (2D model creation, modification, 
visualization, saving to disk, import) should be available on *any* modern Mac OS, 
Linux or Windows system, see [Quick start](#quick-start-and-basic-functionality). 
The bar *to have something
running* is intentionally very low, in order to give as many EIC users as possible 
an opportunity to familiarize themselves with the anticipated EIC Central Detector
layout, acknowledge the limitations, imposed by the accelerator Interaction Region design, 
and contribute to the pre-selection of the EIC Yellow Report detector configuration(s).

  Only basic ROOT scripting knowledge is required for this part. GEANT is not involved. 
Validation tools to estimate very forward and very backward acceptance boundaries
are provided.

  In some sense the provided installation materials may also serve as a concise quick 
start tutorial on how to have ROOT-aware libraries installed and running on your laptop. 
Indeed there is not much specifics about EicToyModel per se in the technical details.

  The [Extended functionaly](#extended-functionality) (interface to GEANT and OpenCascade, 
EicRoot tracker geometry import, VGM interface, etc) requires *controlled environment*, 
and is presented for escalate Docker container and for fun4all singularity container cases. 
Of course, nothing prevents a qualified user from installing everything locally, but only a 
limited support can be provided. That said, a local installation procedure (to first 
order it only involves compiling of VGM and GEANT) is very straightforward. 

<br/>

Other features
--------------

* On-disk model representation (C++ class instance .root file export/import using ROOT 
serializer).
* Dynamic generation of the detector integration volumes GEANT representation.
* Parameteric description of the IR vacuum chamber layout, representing reasonably 
well the available 25mrad crossing angle configuration as of 2020/03/20. As such it 
should be suitable for the *second IR* vacuum chamber description, even that the 
engineering drawings are not available as of yet. 
* Vacuum chamber geometry export in the GDML and ROOT TGeo formats.
* Interface to the BeastMagneticField library (BeAST field map import, as well as
import of the soon-to-become-available magnetic field maps produced by the EIC 
greenfield solenoid design team).
* Export of the integration volumes as a STEP file, for the support structure and 
services engineering design purposes. 

<br/>

Model limitations
-----------------

* The EIC Central Detector is assumed to be composed of exactly four non-overlapping
regions (vertex, barrel, and two endcaps).
* Sub-detector integration volumes in a 2D view of a given region are flat
objects, stacked along the electron beam line axis for the two endcaps and the normal to 
the electron beam line axis for the vertex and barrel stacks. See the picture below. 
* Far-forward "stack" (and B0 magnet in particular) can be added to the model with minor
complications.
* All stacks are organized in a "projective" geometry, which should represent a typical 
layout of a 4pi EIC Central Detector well enough at this stage. 
* A "crack" between the barrel and the endcap stacks in a 2D {Z,R} view is represented by
monotonous functions in both R(Z) and Z(R) representations. "Crack" shape however can 
be configured in a rather flexible way, see examples below.

<br/>

Quick start and basic functionality
-----------------------------------

The installation instructions in this section are to a large extent a gist of the materials 
provided in [FairRoot build pre-requisites](https://fairroot.gsi.de/?q=node/63) and 
[EIC container README](https://gitlab.com/eic/containers), and many more details are 
given there.

Instructions for Windows are not available at this moment (volunteers welcome!), but one should
be able to build the functional bootstrap Docker images for either Centos, or Ubuntu or Fedora (see
the Linux section below) and run the codes in a container environment.

It is indeed not possible to cover every Linux or MacOS installation in a short README. The 
codes were tested on MacOS Catalina (natively), and on Centos 7, Fedora 3.0 and Ubuntu eoan 
(in a Docker container). A number of ROOT versions from 6.14.04 to 6.20.04 were tried out. 
Compilers: from gcc 4.8.5 to gcc 9.3.1, as well as clang 11.0.3 . Various versions of cmake 3.

The instructions below assume the codes will be installed in some newly created user scratch 
directory *\<my-scratch-directory\>* of your choice (like *\$\{HOME\}\/scratch*):

```
mkdir -p <my-scratch-directory>
```

Sounds obvious, yet: replace all instances of *\<my-scratch-directory\>* by the name of your 
actual scratch directory in all the respective commands below.


#### Mac OS

Test environment: MacOS Catalina, Xcode 11.6 (clang 11.0), XQuartz 2.7.7, cmake 3.12.2, binary 
distribution of ROOT 6.20.04 . 

Install the above components using App Store, as well as XQuartz, Kitware and CERN download sites.

There are reasons to believe that any combination of modern MacOS and ROOT should work. Give your
feedback if it does not.

```
# Configure Xcode; this is essential!
xcode-select --install

# Configure ROOT;
. /Applications/root_v6.20.04/bin/thisroot.sh 

# Add cmake to your PATH;
export PATH=/Applications/CMake.app/Contents/bin:${PATH}

# Move to the scratch directory;
cd <my-scratch-directory>
```

From this point on follow the [running](#running-the-scripts) instructions.


#### Linux

Chances are high enough you will be able to run the codes natively on your local host.
Make sure git, cmake, make and your favorite ROOT version are installed. Assuming 
*\<root-installation-directory\>* is where ROOT is installed, configure it:

```
source <root-installation-directory>/bin/thisroot.sh

# Move to the scratch directory;
cd <my-scratch-directory>
```

C-shell users: replace *.sh* by *.csh* in the above command.

From this point on follow the [running](#running-the-scripts) instructions. If this step fails, 
consider to install any missing packages. If uncertain, check *yum* or *apt-get* command lines, 
matching your Linux flavor in a particular Dockerfile [here](doc/docker). Notice that Centos 7
(and consequently Scientific Linux 7) requires a more modern *cmake* to be installed.


#### Docker environment

This is the last resort. The provided [docker files](doc/docker) allow one to build a Docker 
container from scratch, using either Centos 7 or Fedora 3.0 or Ubuntu eoan base images. The 
latter two are preferred (more modern). *sudo* prefix in the commands below is optional, depending 
on the details of your host OS installation.

Obviously only a particular *Dockerfile* in an empty directory is required to build the docker
image. Let's assume one of the three provided Dockerfile's (Centos, Fedore, Ubuntu) was chosen and saved as 
*/tmp/docker/Dockerfile*, either using a text editor, or by cloning the EicToyModel repository
in some temporary area (*git clone https://github.com/eic/EicToyModel.git*) and copying the 
respective file over, or elsewise:

```
# Build the bootstrap image;
sudo docker build -t pre-eictoymodel /tmp/docker

# Run (if your host system is Linux):
sudo docker run --rm -it -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY --user $(id -u) -v <my-scratch-directory>:/scratch pre-eictoymodel
#
# Run (if your host system is Mac OS):
#   XQuartz configuration: go to "Preferences -> Security" and enable "Allow connections from network 
#   clients". Restart XQuartz. Execute 'xhost +localhost' in a terminal.
sudo docker run --rm -it -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=host.docker.internal:0   -v <my-scratch-directory>:/scratch pre-eictoymodel

# (On the container prompt): configure ROOT, move to the /scratch directory:
. /opt/root/bin/thisroot.sh
cd /scratch
```

From this point on follow the [running](#running-the-scripts) instructions.


#### Running the scripts

To this moment we are either in the \<my-scratch-directory\> on a host system (native mode) or in this same 
directory seen as a */scratch* volume in the Docker container. Either way, the remaining sequence of 
commands is the same in all cases:

```
# Download and compile EicToyModel;
git clone https://github.com/eic/EicToyModel.git
cd EicToyModel && mkdir build && cd build
# ! Add the following flag if your local ROOT installation was compiled with -std=c++17:
#   -DCMAKE_CXX_STANDARD=17
cmake -Wno-dev ..
make -j4
```

Edit a script like [example.C](scripts/example.C) in the ../scripts/ directory according to your preferences (see full 
list of the available commands [here](doc/README.API.md)) and run it:

```
# Run the example script;
root -l ../scripts/example.C
```

A ROOT canvas will appear with a picture similar to the image below. 
 
![](doc/sandbox.root.png)

Every colored object in this picture is a sub-detector integration volume. Association 
between ROOT RGB colors and sub-detector tags is 1:1, and (once the palette is agreed upon
by the user community) will never change, and will be exported in this color scheme
to both GEANT and CAD. Current palette is likely not optimal. It is hardcoded 
in [EtmPalette.cc](source/EtmPalette.cc).

Users can create custom detector (or detector group) tags and tag-to-color 
associations like "Calorimetry":kSpring dynamically. For the sake of consistency it may 
make sense to hardcode the "official" Yellow Report color (and detector tag) scheme though, 
after the initial evaluation and debugging stage.

A limited set of interactive commands (see the full list [here](doc/README.API.md)) 
is available. Try e.g. the following sequence with the self-explaining outcome of the 
respective commands: 

```
root [] eic->width(1200); 
root [] eic->mirror();
root [] eic->mirror(false);

# Move the IP to the center of the +/-4.5m area; depending on the actual example.C contents, 
# HCal in the forward endcap may disappear; move it back to some intermediate value;
root [] eic->ip(0);
root [] eic->ip(-30 * etm::cm);

# Vertical and horizontal plane cross cut; in the latter case the 25mrad crossing 
# angle is taken into account; 
root [] eic->vdraw();
root [] eic->hdraw();

# Remove e/m calorimeter from the backward (electron-going) endcap;
root [] eic->bck()->rm("EmCal");
# Install a 20cm deep MPGD "detector" in front of the high-momentum RICH in 
# the hadron-going endcap;
root [] eic->fwd()->insert("MPGD", 20 * etm::cm, "HM RICH");

# Save the current configuration;
root [] eic->write();
# Save example.vc.gdml file with the vacuum chamber layout; a BUG: this command 
# works properly only once per session; 
root [] eic->ExportVacuumChamber();
root [] .q
```

Re-open the produced file with the model in ROOT, if needed:

```
root -l '../scripts/reader.C("example.root")'
```

Beyond this point one can work with the model the same way as if it was created 
from scratch (see [example.C](scripts/example.C)). It is strongly recommended to use 
detector composition changing commands like rm() and insert() only as a quick tuning 
means, and once a desired configuration is found, put the respective changes
in a consistent full script, creating a given model from scratch. The reason 
is simple: this software is in the early debugging stage, and it is much more likely 
than a file format will change rather than the API of the commands will change 
(and the latter can be fixed by hand if needed anyway).

```
root [] auto eic = EicToyModel::Instance(); eic->hdraw();
```

It should be noted that if the geometry was saved using eic->write(*true*) call 
(notice 'true' argument), a naive permanent lock is applied to the contents of the binary 
ROOT file. The geometry can not be modified any longer without hacking the library
(would be a very easy task for a junior C++ programmer, but allows compliant users
to avoid introducing changes into the selected set of the officially distributed 
YR detector configuration files by mistake).


#### Vacuum chamber material and magnetic field scans

  One can perform a magnetic field scan, as well as the vacuum chamber material 
scan at small scattering angles in either e-endcap or h-endcap. This allows one to 
evaluate the potential reach in pseudo-rapidity in both endcaps, as well as to get 
an idea about the azimuthal acceptance asymmetry.

  Magnetic field scan evaluates the effective B*dl integral of the 
transverse-to-trajectory field component along the trajectory of a particle, originated 
at the nominal IP and scattered at a given pair of polar and azimuthal angles. Primary 
vertex smearing along the beam line can be specified.

  The starting point of this scan for a given set of values {z,theta,phi} is the point
where such a particle would exit the accelerator vacuum chamber. Remember, the code knows
the (simplified) IR vacuum chamber layout (actually creates a ROOT TGeo implementation 
dynamically).

  The end point is the most distant from the IP location of the last silicon tracker 
station, which can still be sensibly installed in this detector configuration in a given 
endcap (like in front of the first detector with a lot of material). Defining this 
location is at a discretion of the user. It can be given by a EtmDetectorStack::marker() 
method (see [API description](doc/README.API.md)) when configuring a particular endcap, 
and is indicated e.g. by small red arrows at -230cm and +115cm in the 2D picture above.

  In order to perform a scan on the detector and the vacuum chamber model contained 
in the example.root file produced earlier one can run [this script](scripts/scan.C): 

```
root -l '../scripts/scan.C("example.root")'
```

  Results of this scan are reprsented as three 2D histograms, which can either 
be displayed (see an [example](scripts/scan-viewer.C)) or extracted numerically
using conventional ROOT GetBinContents() calls:

```
root -l '../scripts/scan-viewer.C("example.scan.root")'
```
  A typical set of pictures will look like this:

![](doc/example.scan.png)

  Horizontal and vertical axes on these example plots have a range of +/-60 mrad.
The 360 degree azimuthal angle scan is given for a range of pseudo-rapidities 
between -4.5 and -3.5 (electron-going endcap). A green dashed line corresponds 
to the pseudo-rapidity of -4.0 . A square outline seen in the pictures corresponds
to the profile of the aluminum portion of the beam pipe (has to be changed in the 
next iteration of the design).

  Assuming a single-coil solenoid with its axis aligned with the electron beam line, 
it is not unreasonable to assume that the magnetic field will be shaped up in a way 
the lines are parallel to the axis in the whole volume of the forward and backward 
silicon trackers, in order to maximize the average B*dl integral. Therefore the 
constant field approximation should give one a pretty good idea about the overall 
bending power (and consequently, together with the linear space available for the 
tracker, the momentum resolutions), since the optimal tracker configuration is a set 
of N equidistant disks in this case. To first order one does not even need to run 
a full GEANT simulation + reconstruction chain to obtain a reasonable estimate.

  If somebody knows a way how to make DrawFrame() and a polar 2D histogram Draw() 
live together and allow one to use axis lables and proper title fonts, such an 
advice will be greatly appreciated.


<br/>

GEANT interface
---------------

Typically one would only use GEANT interface and other extended features in a well-defined 
(and maintained!) environment, like an escalate Docker container, a fun4all singularity 
container, or a RACF farm at BNL.

GEANT has to be installed (either on the local system or in a container environment) and 
its configuration file must be sourced, like 

```
source <geant4-installation-directory>/bin/geant4.sh
```

C-shell users will have to replace *.sh* by *.csh* in the above command. Be aware that 
this command is executed in both escalate and fun4all container environment setup scripts, 
see below.

GEANT-related cmake command line keys are:

```
#
# if GEANT executable(s) from the 'examples' directory will be compiled (be aware, "." works):
#   -DCMAKE_INSTALL_PREFIX=<EicToyModel-installation-directory>
#
# if GEANT4 interface is required:
#   -DGEANT=YES
#
# if the IR vacuum chamber shape boolean cut through the integration volumes is required:
#   -DVGM=<VGM-installation-directory>
#
# Be aware that LD_LIBRARY_PATH should contain the location of the VGM libraries, if the 
# respective interface is compiled in;

# Help standalone executables (g4e, fun4all root.exe, codes provides in 'examples' directory)
# find respective include and library files;
make install
```

Apart from the eic->ExportVacuumChamber() command shown above, one can create G4 
detector integration volumes on the fly, either one at a time or all of them at 
once. See a short example executable [main.cc](examples/basic/main.cc) as an example. 

The library should be configured with the -DGEANT cmake command line key (see above).

The integration volumes are represented by either G4GenericPolycone or G4Polyhedra 
shapes. An azimuthally asymmetric boolean cut by the vacuum chamber shape is applied.

Naming convention for these volumes may require some tuning. At present e.g. a second
from the IP TRD detector volume in the forward endcap will be named as "FWD.TRD.01", 
and so on, with the stack identifiers "VTX", "MID", "BCK" and "FWD" for the vertex, 
barrel, e-endcap and h-endcap stacks, respectively, and the detector tags as hardcoded 
in [EtmPalette.cc](source/EtmPalette.cc). This may as well be irrelevant, since a user
can poll the G4VPhysicalVolume pointer for a particular detector by means of the 
EtmDetector::GetG4Volume() method.

Once a user gets access to a particular logical volume, he/she can populate this volume 
with the daughter objects, observing the usual GEANT rules and volume boundary conditions. 
Presently the local coordinate system of the G4GenericPolycone (G4Polyhedra) volumes
is defined in the following way:

* In the endcaps the volumes are "shifted to 0", and there is no 180 rotation around 
vertical axis for the electron-going endcap objects. This e.g. means (see the above picture)
that in the local G4GenericPolycone coordinate system, the upstream face of the 
e-endcap HCal will be located at +52.5cm and the downstream face at -52.5cm, where 
upstream/downstream is counted along the *electron* beam direction. In other words, 
calorimeter towers in this volume need to be placed centered at 0 along the beam line 
direction, and the container volume will be shifted to -3.5m as a whole without any rotation.

* In the vertex and the barrel stacks the G4GenericPolycone volumes are centered around
the IP rather than around their (Zmax+Zmin)/2 geometric center. In other words, an object 
placed at (0,0,0) in their local coordinate system, will be physically placed at (IP,0,0)
in the world volume.

 This way, to first order, whatever daughter objects are placed inside the integration volumes, 
the geometry will be consistent after moderate re-shuffling of a particular detector 
stack (say, after removing one of the TRD volumes in the hadron-going endcap, the e/m 
calorimeter behind it will be re-located to a proper place in the world volume). 

It seems to be wise to check the integration volume actual location by means of the 
available library calls, and *tune* the sub-detector geometry accordingly if needed.
For instance, populate the endcap HCal integration volume by towers depending on the
actually available radial space (*radial size: TODO list*).

Apparently the community may want to decide exporting *individual* GDML objects
describing the sub-detector integration volumes for a given version of the full EIC detector
geometry, therefore avoiding the dependency on either the ETM library described here 
or on the ROOT itself in the GEANT environment alltogether. Providing consistency 
between different sub-detector systems may be problematic in this case though.

The [example](examples) directory contains a couple of simple standalone codes, 
with their own CMakeLists.txt files, which illustrate the usage. See the bare minimum 
GEANT example source code [here](examples/basic/main.cc). 

A full sequence of actions required to have this basic GEANT example running in both 
*excalate* and *fun4all* container environments is shown below.


#### *escalate* Docker container

See [Docker environment hints](#docker-environment) in the previous section if the sequence 
outlined below does not work for you. It is assumed that a local *\<my-scratch-directory\>*
was created already. 

As of 2020/07/21 the container has gcc 9.2.1, ROOT 6.20.04 and GEANT 10.6.01 .

```
# Run (if your host system is Linux):
sudo docker run --rm -it -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY --user $(id -u) -v <my-scratch-directory>:/scratch electronioncollider/escalate /bin/bash
#
# Run (if your host system is Mac OS):
sudo docker run --rm -it -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=host.docker.internal:0   -v <my-scratch-directory>:/scratch electronioncollider/escalate /bin/bash

# The rest happens inside the running container:
cd /scratch

# Download the EicToyModel codes, compile, run example.C (the latter will create example.root file 
# with the intergation volume configuration);
git clone https://github.com/eic/EicToyModel.git
cd EicToyModel && mkdir build && cd build
# ! modify according to the current escalate container contents;
cmake -DCMAKE_INSTALL_PREFIX=. -DGEANT=YES -DVGM=/container/app/vgm/vgm-v4-5 -Wno-dev ..
```
Beyond that point there is no difference between escalate and fun4all environment, [follow the instructions here](#common-part).


#### *fun4all* singularity container

It is assumed of course that cvmfs support is configured and singularity executable is installed
on the local host. 

As of 2020/07/21 the container has gcc 4.8.5, ROOT 6.16.00 and GEANT 10.2.02, and also does not have VGM 
installed (so the integration volumes in this example will not have a boolean cutaway by the vacuum chamber 
volume).

```
singularity shell -B /cvmfs:/cvmfs -B <my-scratch-directory>:/scratch /cvmfs/sphenix.opensciencegrid.org/singularity/rhic_sl7_ext.simg

source /cvmfs/sphenix.opensciencegrid.org/x8664_sl7/opt/sphenix/core/bin/sphenix_setup.sh

cd /scratch
git clone https://github.com/eic/EicToyModel.git
cd EicToyModel && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=. -DGEANT=YES -Wno-dev ..
```

Beyond that point there is no difference between escalate and fun4all environment, [follow the instructions here](#common-part).

#### Common part

```
make -j4 install
root -l ../scripts/example.C

export LD_LIBRARY_PATH=/scratch/EicToyModel/build/lib:${LD_LIBRARY_PATH}

# Compile a standalone GEANT executable and run it (import example.root file, create the 
# 3D integration volumes on the fly and place them into the "IR world" volume); 
cd ../examples/basic
mkdir build && cd build
cmake -DETM=/scratch/EicToyModel/build -Wno-dev ..
make -j4
./basic ../../../build/example.root
```

This sequence brings up a GEANT4 Qt event display with the 3D integration volume configuration 
described by the created earlier 2D cartoon. A BUG: in the fun4all case click on "Useful tips" 
first, then click back on "viewer-0".

Working examples of the toy model integration volumes usage in the actual escalate and fun4all
EIC detector geometry description will follow shortly.

<br/>

Extended functionality
----------------------

The additional cmake command line keys are provided below:

```
#
# for CAD export functionality:
#   -DOPENCASCADE=<OpenCascade-installation-directory>
#
# for magnetic field map interface:
#   -DBFIELD=<BeastMagneticField-installation-directry>
#
# Be aware that LD_LIBRARY_PATH should contain the locations of the OpenCascade,
# BeastMagneticField, if the respective interfaces are compiled in;
```

#### CAD interface

This interface may be useful to export the created models in a STEP format.

The library has to be compiled with a -DOPENCASCADE=\<OpenCascade-installation-dir\> cmake
flag (and obviously OpenCascade libraries must be installed on a local system). 

7.2.0 is known to work.

One can either export a CAD model with all of the integration volumes at once by using 
EicToyModel::Export() method, or with a single detector volume by using EtmDetector::Export() 
method, see [README.API.md](doc/README.API.md) for more details.

One can also store a TObjString with a CAD model of all of the integration volumes in 
a .root file (see EtmDetector::Export() method parameters), where from it can be retrieved
by a separate [script](scripts/extract-cd-step.C) *without a need to have OpenCascade
libraries installed on a local system*.


#### Magnetic field interface

A small [BeastMagneticField](https://eic.github.io/software/beast_magnetic_field.html) library 
is used to import BeAST model detector magnetic field map. The library has to be installed locally. 
The instructions are available [here](https://github.com/eic/BeastMagneticField). 

BeastMagneticField library is meant to be compatible with any new field maps, which a joint BNL/JLAB
magnet designer team will produce in the course of the YR/CDR work on the EIC greenfield central 
detector solenoid.

The EicToyModel library has to be configured with cmake -DBFIELD flag, see the full list of 
keys at the beginning of this README.

If the EicToyModel library is configured *without* BeastMagneticField interface, one can still 
perform the B*dl integral scans, assuming constant field, parallel to the electron beam line
axis.

<br/>

ROOT macro options
------------------

Full current list of options is available [here](doc/README.API.md).