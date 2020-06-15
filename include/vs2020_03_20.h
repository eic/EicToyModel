
#include <EtmVacuumSystem.h>

#ifndef _VS2020_03_20_
#define _VS2020_03_20_

class vs2020_03_20: public EtmVacuumSystem {
 public:
 vs2020_03_20( void );
  ~vs2020_03_20() {};

  void CreateGeometry( void );
  bool ConfigurableCrossingAngle( void ) const { return true; };//false; };
  //double FixedCrossingAngle     ( void ) const;

  G4VSolid *CutThisSolid(G4VSolid *solid, const std::vector<TVector2> &polygon);

  ClassDef(vs2020_03_20, 1)
};

#endif
