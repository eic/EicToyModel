
#ifndef _EICROOT_MUMEGAS_SUBSYSTEM_
#define _EICROOT_MUMEGAS_SUBSYSTEM_

#include <EicRootSubsystem.h>
#include <MuMegasGeoParData.h>

#include <map>

class EicRootMuMegasSubsystem : public EicRootSubsystem, public MuMegasGeoParData
{
 public:
  // ctor & dtor;
 EicRootMuMegasSubsystem(const std::string& name): EicRootSubsystem(name) {};
  ~EicRootMuMegasSubsystem() {};

  // FIXME: should find a more natural way to arrange this;
  void PlaceG4Volume(G4LogicalVolume *mother, bool check = false, 
		     void *pRot = 0, void *tlate = 0) {
    EicGeoParData::PlaceG4Volume(mother, check, pRot, tlate);
  };
  const std::map<G4VPhysicalVolume*, unsigned>  &GetG4SensitiveVolumes( void ) const {
    return EicGeoParData::GetG4SensitiveVolumes();
  };
};

#endif
