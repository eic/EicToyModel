
#### Tracking with EicRoot shapes in the *fun4all* singularity container

  The sequence of commands below should work out of the box, as long as the local system 
is configured with singularity and CVMFS support.

```
# Start singularity container; mount /cvmfs volumes;
singularity shell -B /cvmfs:/cvmfs -B <your-scratch-directory>:/scratch /cvmfs/eic.opensciencegrid.org/singularity/rhic_sl7_ext.simg

# Run EIC-specific fun4all configuration script;
source /cvmfs/eic.opensciencegrid.org/x8664_sl7/opt/fun4all/core/bin/eic_setup.sh -n

#
# It is assumed that 'cd /scratch && git clone https://github.com/eic/EicToyModel.git && mkdir -p /scratch/EicToyModel/build'
# happened already; do it now otherwise;
#

# Configure EicToyModel with G4 and VGM interfaces built in; compile it;
cd /scratch/EicToyModel/build
cmake -DCMAKE_INSTALL_PREFIX=. -DGEANT=YES -Wno-dev -DVGM=${OPT_SPHENIX}/vgm ..
make -j2 install

# Append missing directories to LD_LIBRARY_PATH;
export LD_LIBRARY_PATH=/scratch/EicToyModel/build/lib:${OPT_SPHENIX}/vgm/lib64:${LD_LIBRARY_PATH}

# Create a simple integration volume model (eicroot.root) to be imported in fun4all;
root -l ../scripts/eicroot.C

# Configure and compile whatever is required for fun4all scripts;
cd /scratch/EicToyModel/fun4all_with_eicroot && mkdir -p build && cd build
../sandbox/autogen.sh --prefix=/scratch/EicToyModel/fun4all_with_eicroot

# Go to the macro directory and run them; ignore the G4 and ROOT warnings;
source /cvmfs/eic.opensciencegrid.org/x8664_sl7/opt/fun4all/core/bin/setup_local.sh /scratch/EicToyModel/fun4all_with_eicroot
export ROOT_INCLUDE_PATH=/scratch/EicToyModel/build/include/etm:${ROOT_INCLUDE_PATH}
cd ../macro

# This shows a Qt display with a single VST cylinder embedded into the (central) TRACKER integration volume;
root -l Fun4All_G4_QtDisplay.C

# This runs 1000 events through a simple tracker consisting of four silicon layers modeled
# according to the ALICE ITS2 description, and shows a momentum resolution plot; 
root -l Fun4All_G4_Tracking.C
root -l g4eval.root
root [1] _h1d_Delta_mom->Fit("gaus");
```

See [Fun4All_G4_QtDisplay.C](macro/Fun4All_G4_QtDisplay.C) and [Fun4All_G4_Tracking.C](macro/Fun4All_G4_Tracking.C)
for more details. 