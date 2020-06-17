
This section provides a list of the C++ methods available to users either from the 
ROOT prompt or in the GEANT application or in any other environment where a .root 
file with an EicToyModel class instance was imported.

Be aware that for a better readability several method names have only a shortcut 
representation (e.g. there is a method EtmDetectorStack::add(), but no 
EtmDetectorStack::AddNewDetector() one; EicToyModel::fwd(), but no 
EicToyModel::ForwardEndcap() one, and so on). 

EicToyModel class methods
-------------------------

  Several methods return *this* pointer, which allows one to chain several commands 
in one line. Length units are expected in [cm], and one can explicitly multiply the 
arguments by etm::mm, etm:cm, etm:m for clarity (a la GEANT convention). Be aware
that etm::cm numerically equals to 1.0 (ROOT convention), compare to 'cm=10.0' in GEANT.

  *redraw* argument in several commands below: if "true", redraw the scene.

  Coordinate system is such that {Z,R}=(0,0) is the center of the +/-4.5m area and +Z
axis is pointing in the hadron-going direction.

#### Generic IR layout description 

```
  EicToyModel *acceptance(double eta0, double eta1, double eta2, double eta3, 
			  bool reset_stacks = false, bool redraw = true);

  Defines acceptance boundaries:

  eta[0123]   : (eta0)backward-(eta1)-barrel-(eta2)-forward(eta3) pseudo-rapidity 
acceptance boundaries
  reset_stacks: if "true", stack detector composition will be erased
```

```
  EicToyModel *DefineVacuumChamber(EtmVacuumChamber *vc);

  Defines vacuum chamber layout:

  vc          : a derived class describing the IR vacuum chamber geometry
```

```
  EicToyModel *ip(double offset, bool redraw = true);

  Specifies nominal IP shift:

  offset      : nominal IP offset with respect to the Z=0 point in the middle of the 
+/-4.5m area available for the central detector
```

```
  EicToyModel *ir(double length, double radius, bool redraw = true);

  Defines overall scene size:

  length      : allowed IR region length along the electron beam line; +/- 4.5m per default
  radius      : allowed IR region radial size; 4.0m per default
```

```
  EicToyModel *SetCrossingAngle(double value, bool redraw = true);

  Defines crossing angle (0.025 - aka 25 mrad - per default):

  value       : crossing angle in [rad]
```

```
  TVector2 GetIpLocation( void );

  Returns the IP location (where X-coordinate is the IP shift and Y-coordinate is 0).
```

```
  double GetCrossingAngle( void )      const { return mCrossingAngle; };
  double GetIrRegionLength( void )     const { return mIrRegionLength; };
  double GetIrRegionRadius( void )     const { return mIrRegionRadius; };

  Access methods: crossing angle in [rad], IR view area (full length along the beam 
line and max radius), in [cm].
```

```
  void Construct( void );

  Build the detector geometry (polygon collection in 2D) based on the current 
internal structure of the EicToyModel class instance data fields, as well as the 
stack composition and individual sub-detector class data fields.
```

```
  void PlaceG4Volumes(G4LogicalVolume *world);

  Place all the sub-detector container volumes one by one in the provided GEANT 
world volume.
```

```
  void write(bool lock = false);

  Export the current state of the model as a <model-name>.root file with an EicToyModel
instance, using standard ROOT serializer.

  lock        : if "true", a flag will be set in the EicToyModel class data, which effectively
                only allows one to import it in a read-only mode. May be useful for .root file
                distribution purposes. Apparently, one can deliberately add a method to the 
                library, which resets the lock, but a compliant user will not be able to make 
                any further changes to the actual geometry (as saved in the polygon structure).

  write() is equivalent to Export("<model-name>.root", false). 
```

```
  void Export(const char *fname, bool lock = false);

  Export the current EIC detector geometry either as a ROOT file ("fname" has
extension .root) or as a CAD model (extension .stp) . ROOT file will also contain 
a TGeo representation of the vacuum chamber geometry. 

  fname       : output file name 
  lock        : if "true", geometry will appear to be locked (immutable) upon import.
```

```
  void ExportVacuumChamber(const char *fname = 0);

  Export TGeo model of the vacuum chamber geometry. 

  fname       : output file name. If 0, defaults to "<model-name>.vc.root" .
```

#### Detector stack access

```
  EtmDetectorStack *vtx( void ); EtmDetectorStack *vertex  ( void ) { return vtx(); };
  EtmDetectorStack *bck( void ); EtmDetectorStack *backward( void ) { return bck(); };
  EtmDetectorStack *mid( void ); EtmDetectorStack *barrel  ( void ) { return mid(); };
  EtmDetectorStack *fwd( void ); EtmDetectorStack *forward ( void ) { return fwd(); };

  return pointer to the vertex/e-endcap/barrel/h-endcap detector stacks, respectively
```

#### Visualization

```
  EicToyModel *width(unsigned width);

  width       : ROOT canvas width in screen pixels   
```

```
  EicToyModel *mirror(bool what = true, bool redraw = true);

  Turn mirror image on and off. They are not identical in the horizontal cut view 
because of the 25mrad crossing angle.

  what        : if "true", turn mirror image on
```

```
  EicToyModel *full (bool redraw = true);
  EicToyModel *left (bool redraw = true);
  EicToyModel *right(bool redraw = true);

  Draw full view, left hand side (e-endcap), right hand side (h-endcap)
```

```
  EicToyModel *legend(bool what = true, bool redraw = true);

  Turn legend on / off:

  what        : if "true", legend is on
```

```
  void vdraw( void );
  void hdraw( void );
  void  draw( void );

  Display vertical, horizontal or current cross cut view, respectively.
```

```
  void home(bool redraw = true);

  Camera home.
```

```
  void zoom(double blX, double blY, double trX, double trY, bool redraw = true);

  Display zoomed view of a selected area. Units are in [cm]:

  blX,blY     : XY-coordinates of the bottom left corner
  trX,trY     : XY-coordinates of the top right corner
```

```
  EicToyModel *AddEtaLine(double value, bool line = true, bool label = true, bool redraw = true);

  Display a particular pseudo-rapidity line:

  eta         : pseudo-rapidity value
  line        : if "true", draw the respective straight line
  label       : if "true", draw the label
```

```
  EicToyModel *RemoveEtaLine(double eta);

  May want to remove a given pseudo-rapidity line from the picture (overlapping labels, etc.):

  eta         : pseudo-rapidity value
```

```
  void ApplyStandardTrimming( void );

  Beautify the displayed container shapes a bit. Be aware that this option actually 
eats part of the space away from the exported integration volumes.
```

#### Other


```
  EicToyModel *SetName(const char *name);

  Set model name.
```

```
  EicToyModel *DrawFlatFieldLines(double eta);

  Hard to explain.
```

```
  static EicToyModel *Instance( void );

  Returns EicToyModel singleton class instance.
```



EtmDetectorStack class methods
------------------------------


EtmDetector class methods
-------------------------