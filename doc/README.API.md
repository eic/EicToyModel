
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

```
  EicToyModel *acceptance(double eta0, double eta1, double eta2, double eta3, 
			  bool reset_stacks = false, bool redraw = true);

  eta[0123]   : (eta0)backward-(eta1)-barrel-(eta2)-forward(eta3) pseudo-rapidity 
acceptance boundaries

  reset_stacks: if "true", stack detector composition will be erased

  redraw      : if "true", redraw the scene
```



EtmDetectorStack class methods
------------------------------


EtmDetector class methods
-------------------------