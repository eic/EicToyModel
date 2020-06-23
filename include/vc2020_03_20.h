
#include <EtmVacuumChamber.h>

#ifndef _VC2020_03_20_
#define _VC2020_03_20_

class vc2020_03_20: public EtmVacuumChamber {
 public:
 vc2020_03_20( void );
  ~vc2020_03_20() {};

  void CreateGeometry( void );
  bool ConfigurableCrossingAngle( void ) const { return true; };//false; };
  //double FixedCrossingAngle     ( void ) const;

  ClassDef(vc2020_03_20, 1)
};

#endif
