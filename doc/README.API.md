
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
in one line.

### Generic IR layout description 

```
  EicToyModel *acceptance(double eta0, double eta1, double eta2, double eta3, 
			  bool reset_stacks = false, bool redraw = true);

  eta[0123]   : (eta0)backward-(eta1)-barrel-(eta2)-forward(eta3) pseudo-rapidity 
acceptance boundaries

  reset_stacks: if "true", stack detector composition will be erased

  redraw      : if "true", redraw the scene
```

```
  EicToyModel *DefineVacuumChamber(EtmVacuumChamber *vc);

  vc          : a derived class describing the IR vacuum chamber geometry
```

```
  EicToyModel *ip(double offset, bool redraw = true);

  offset      : nominal IP offset with respect to the Z=0 point in the middle of the 
+/- 4.5m area available for the central detector

  redraw      : if "true", redraw the scene
```

```
  EicToyModel *ir(double length, double radius, bool redraw = true);

  length      : allowed IR region length along the electron beam line; +/- 4.5m per default

  radius      : allowed IR region radial size; 4.0m per default

  redraw      : if "true", redraw the scene
```

### Detector stack access

```
  EtmDetectorStack *vtx( void ); EtmDetectorStack *vertex  ( void ) { return vtx(); };
  EtmDetectorStack *bck( void ); EtmDetectorStack *backward( void ) { return bck(); };
  EtmDetectorStack *mid( void ); EtmDetectorStack *barrel  ( void ) { return mid(); };
  EtmDetectorStack *fwd( void ); EtmDetectorStack *forward ( void ) { return fwd(); };

  return pointer to the vertex/e-endcap/barrel/h-endcap detector stacks, respectively
```




EtmDetectorStack class methods
------------------------------


EtmDetector class methods
-------------------------