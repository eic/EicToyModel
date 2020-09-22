
#ifndef _EICROOT_GEM_SUBSYSTEM_
#define _EICROOT_GEM_SUBSYSTEM_

#include <EicRootSubsystem.h>
#include <GemGeoParData.h>

#include <map>

class EicRootGemSubsystem : public EicRootSubsystem, public GemGeoParData
{
 public:
  // ctor & dtor;
 EicRootGemSubsystem(const std::string& name): EicRootSubsystem(name) {};
  ~EicRootGemSubsystem() {};

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
