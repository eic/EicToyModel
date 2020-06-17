
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

  Coordinate system is chosen such that {Z,R}=(0,0) is the center of the +/-4.5m area and +Z
axis is pointing in the hadron-going direction.

  Pseudo-rapidity values are converted to [mrad] as angles with respect to the outgoing 
electron beam line direction for the e-endcap and angles with respect to the outgoing hadron 
beam line direction for the h-endcap (account for the crossing angle). Be aware, that the 
crossing angle is *not* accounted for the barrel-to-hadron-endcap boundary.


#### Generic IR layout description 

```
  EicToyModel *acceptance(double eta0, double eta1, double eta2, double eta3, 
			  bool reset_stacks = false, bool redraw = true);

  Defines the acceptance boundaries:

  eta[0123]   : (eta0)backward-(eta1)-barrel-(eta2)-forward(eta3) pseudo-rapidity 
acceptance boundaries
  reset_stacks: if "true", stack detector composition will be erased

  Default pseudo-rapidity values: [-4, -1, +1, +4].
```

```
  EicToyModel *DefineVacuumChamber(EtmVacuumChamber *vc);

  Defines the vacuum chamber layout:

  vc          : a derived class describing the IR vacuum chamber geometry

  As of 2020/06/16 only the vc2020_03_20() model is available.
```

```
  double GetCrossingAngle( void )      const { return mCrossingAngle; };
  double GetIrRegionLength( void )     const { return mIrRegionLength; };
  double GetIrRegionRadius( void )     const { return mIrRegionRadius; };

  Access methods: crossing angle in [rad], IR view area (full length along the beam 
line and max radius), both in [cm].
```

```
  TVector2 GetIpLocation( void );

  Returns the IP location (where X-coordinate is the IP shift and Y-coordinate is 0).
```

```
  EicToyModel *ip(double offset, bool redraw = true);

  Specifies the nominal IP shift:

  offset      : nominal IP offset with respect to the Z=0 point in the middle of the 
+/-4.5m area, available for the central detector
```

```
  EicToyModel *ir(double length, double radius, bool redraw = true);

  Defines the overall scene size:

  length      : allowed IR region length along the electron beam line; +/- 4.5m per default
  radius      : allowed IR region radial size; 4.0m per default
```

```
  EicToyModel *SetCrossingAngle(double value, bool redraw = true);

  Defines crossing angle (0.025 - aka 25 mrad - per default):

  value       : crossing angle in [rad]
```

#### File export

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

  Export GDML model of the vacuum chamber geometry. 

  fname       : output file name. If 0, defaults to "<model-name>.vc.gdml" .
```

```
  void write(bool lock = false);

  Export the current state of the model as a <model-name>.root file with an EicToyModel
instance, using standard ROOT serializer.

  lock        : if "true", a flag will be set in the EicToyModel class data, which effectively
                only allows one to import it in a read-only mode. May be useful for .root file
                distribution purposes. Apparently, one can deliberately add a method to the 
                library, which resets the lock, but a compliant user will not be able to make 
                any further changes to the actual geometry (as saved in the polygon structure)
                accidentally.

  write() is equivalent to Export("<model-name>.root", false). 
```

#### Model import

```
  void Construct( void );

  Build the detector geometry (a collection of polygons in 2D), based on the current 
contents of the EicToyModel class instance data fields, as well as the 
stack composition and individual sub-detector class data fields.
```

```
  void PlaceG4Volumes(G4LogicalVolume *world);

  Place all the sub-detector container volumes one by one in the provided GEANT 
world volume. See scripts/viewer.C example script how to extract it. 

  world       : GEANT world logical volume 
```

  See also ![reader.C](../scripts/reader.C), ![main.cc](../source/main.cc) and
![viewer.C](../scripts/viewer.C) files.


#### Detector stack access

```
  EtmDetectorStack *vtx( void ); EtmDetectorStack *vertex  ( void ) { return vtx(); };
  EtmDetectorStack *bck( void ); EtmDetectorStack *backward( void ) { return bck(); };
  EtmDetectorStack *mid( void ); EtmDetectorStack *barrel  ( void ) { return mid(); };
  EtmDetectorStack *fwd( void ); EtmDetectorStack *forward ( void ) { return fwd(); };

  Return pointer to the vertex / e-endcap / barrel / h-endcap detector stacks, respectively.
```

#### Visualization

```
  EicToyModel *AddEtaLine(double value, bool line = true, bool label = true, bool redraw = true);

  Display a particular pseudo-rapidity line on the picture (like eta=0.0, which is neither 
of the acceptance boundaries):

  eta         : pseudo-rapidity value
  line        : if "true", draw the respective straight line
  label       : if "true", draw the label
```

```
  void ApplyStandardTrimming( void );

  Beautify the displayed container shapes a bit. Be aware that this option actually 
eats part of the space (edges) away from the exported integration volumes.
```

```
  EicToyModel *full (bool redraw = true);
  EicToyModel *left (bool redraw = true);
  EicToyModel *right(bool redraw = true);

  Draw either the full view, or the left hand side (e-endcap), or the right hand side 
(h-endcap).
```

```
  void home(bool redraw = true);

  Camera home. Useful to escape from a zoom view.
```

```
  EicToyModel *legend(bool what = true, bool redraw = true);

  Turn legend on / off:

  what        : if "true", legend is on (default)
```

```
  EicToyModel *mirror(bool what = true, bool redraw = true);

  Turn mirror image on and off. Two halves are not identical in the horizontal cut view 
because of the 25mrad crossing angle.

  what        : if "true", turn mirror image on
```

```
  EicToyModel *RemoveEtaLine(double eta);

  May want to remove a given pseudo-rapidity line from the picture (overlapping labels, etc.):

  eta         : pseudo-rapidity value
```

```
  void vdraw( void );
  void hdraw( void );
  void  draw( void );

  Display vertical, horizontal or current cross cut view, respectively.
```

```
  EicToyModel *width(unsigned width);

  Set ROOT TCanvas width. Canvas height will be recalculated based on the current 
active area aspect ratio in the world coordinates.

  width       : ROOT canvas width in screen pixels   
```

```
  void zoom(double blX, double blY, double trX, double trY, bool redraw = true);

  Display zoomed view of a selected area. Units are in [cm]:

  blX,blY     : XY-coordinates of the bottom left corner
  trX,trY     : XY-coordinates of the top right corner
```

#### Other


```
  EicToyModel *DrawFlatFieldLines(double eta);

  Hard to explain. 
```

```
  static EicToyModel *Instance( void );

  Returns EicToyModel singleton class instance.
```

```
  EicToyModel *SetName(const char *name);

  Set model name.
```

EtmDetectorStack class methods
------------------------------

```
  EtmDetector *add(const char *tag,            double length);
  EtmDetector *add(const char *tag, int color, double length);

  Add a detector described by a tag (like "TRD") to the current stack, behind the 
current last detector.

  tag         : either one of the hardcoded tags (see source/EtmPalette.cc)
                or a dynamically defined tags, defined by an add(tag,color,length)
                earlier; color will be taken out of the lookup table
  color       : a unique ROOT color index associated with this tag
  length      : detector "depth" along the alignment axis, in [cm]
```

```
  EtmDetector *gap(double length);

  Add a spacer between the actual detector integration volumes.

  length      : gap length along the alignment axis, in [cm]
```


```
  EtmDetector *marker( void );

  Add a special fake detector at the current location inside the stack. It will be 
used in B*dl integral and space-for-silicon-tracker calculations for a given endcap.
Should be placed at the location where one believes the last silicon tracker station 
(counting from the IP) may be realistically located (like in front of the first 
detector with a large material budget).  
``` 

```
  EtmDetector *insert(const char *tag, double length, const char *after, unsigned order = 0);

  Insert a detector with a known tag, behind (?) some other detector, which was already 
defined in this stack.  

  tag         : one of the already defined detector tags (like "MPGD")
  length      : detector "depth" along the alignment axis, in [cm]

 {after,order} pair is required because there can be more than one detector of a given 
type (like a TRD) in one stack.

  This command is only of interest for interactive work. 
```

```
  void rm(const char *dname);
  void rm(const char *dname, unsigned order);

  Remove a unique detector with tag 'dname' or a detector specified by a {dname,order}
pair in case more than one detector of this type exists in this stack.

  This command is only of interest for interactive work. 
```

```
  void offset(double dstart = 0.0);

  Change the offset of the very first detector of this stack (moves the whole 
stack at once either toward sor away from the IP).

  dstart      : offset in [cm]     
```

```
  EtmDetector *get(const char *dname, unsigned order = 0) const;

  Get pointer to a detector with a tag 'dname' in this stack (or a {dname,order} pair).
```

EtmDetector class methods
-------------------------

```
  EtmDetector *trim(double min, double max, bool preserve_modified = false);

  Modify the default container volume shape layout at the eta=min and eta=max
acceptance boundaries (min:max pair is given in ascending order!). Values are 
betweek 0.0 and 1.0, and they determine, which portion of a volume polygon tip
will be cut to make it look better. 0.0 value should be used to allocate as much 
of the available space to the container volume as other boundary conditions allow.

  EtmDetector *trim(double value) { return trim(value, value, false); };
  EtmDetector *brick( void ) { return trim(0.0); };

  Shortcuts to the above trim(min,max,preserve_modified) method.
```

```
  double length( void ) const;
  void length(double value);

  Change and get the detector "depth" along the stack alignment axis, in [cm].
```
```
  EtmDetector *stretch(EtmDetector *refdet,        double toffset, etm::Stretch how = etm::kRecess);
  EtmDetector *stretch(EtmDetector *refdet,                        etm::Stretch how = etm::kRecess);
  EtmDetector *stretch(                            double tlength, etm::Stretch how = etm::kRecess);
  EtmDetector *stretch(EtmDetectorStack *refstack, double tlength, etm::Stretch how = etm::kRecess);

  Various ways to define the length of a particular detector along the lateral direction with respect to the stack alignment axis. One can either defined it with respect to some other detector in a neighboring stack or in absolute numbers. The former option is preferred, since the detector configuration then becomes more flexible in a sense the detectors move synchronously when something changes in oe of the other stacks (like its offset with respect to the IP). 

  how         : defines a way the crack between the detector stacks (say between e-endcap and barrel) gets modified; possible values are: etm::kDent, etm::kRecess and etm::kWall; try them out. 
```

```
  double GetActualDistance( void ) const;

  Returns the actual distance of the center of this detector volume to the IP, along the stack 
alignment axis.
```

```
  bool IsDummy( void ) const;

  Returns 'true' if this is either a MARKER or GAP pseudo-detector.
```

```
  G4VPhysicalVolume *PlaceG4Volume(G4LogicalVolume *world, const char *name = 0);

  Places a GEANT physical volume into the world volume either under a given name or 
under a name composed of the stack label, detector tag and detector order of this type in 
a given stack.

  Returns respective G4VPhysicalVolume. 
```

```
  void Export(const char *fname);

  Exports a CAD file with this single container volume. This method should probably be 
removed, see EicToyModel::Export() call.
```