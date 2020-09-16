
#ifndef _EICROOT_VST_SUBSYSTEM_
#define _EICROOT_VST_SUBSYSTEM_

#include <EicRootSubsystem.h>
#include <VstGeoParData.h>

class EicRootVstSubsystem : public EicRootSubsystem, public VstGeoParData
{
 public:
  // ctor & dtor;
 EicRootVstSubsystem(const std::string& name): EicRootSubsystem(name) {};
  ~EicRootVstSubsystem() {};

  // FIXME: should find a more natural way to arrange this;
  void PlaceG4Volume(G4LogicalVolume *mother, bool check = false, 
		     void *pRot = 0, void *tlate = 0) {
    EicGeoParData::PlaceG4Volume(mother, check, pRot, tlate);
  };
  const std::vector<G4VPhysicalVolume*> &GetG4SensitiveVolumes( void ) const {
    return EicGeoParData::GetG4SensitiveVolumes();
  };
};

#endif
