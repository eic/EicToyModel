// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef _EICROOT_SUBSYSTEM_
#define _EICROOT_SUBSYSTEM_

#include <g4detectors/PHG4DetectorSubsystem.h>

#include <vector>
#include <map>


class EicRootDetector;
class PHCompositeNode;
class PHG4Detector;
class PHG4SteppingAction;

class G4LogicalVolume;
class G4VPhysicalVolume;

class EicRootSubsystem : public PHG4DetectorSubsystem
{
 public:
  // ctor & dtor;
  EicRootSubsystem(const std::string& name);
  virtual ~EicRootSubsystem() {};

  // Initialization;
  int InitRunSubsystem(PHCompositeNode*) override;

  // Event processing;
  int process_event(PHCompositeNode*) override;

  // Accessor (reimplemented)
  PHG4Detector* GetDetector() const override;

  PHG4SteppingAction* GetSteppingAction() const override { return m_SteppingAction; }

  // Print info (from SubsysReco);
  void Print(const std::string& what = "ALL") const override;

  const std::string &GetG4HitName( void ) const { return m_G4HitName; };

  // These two methods will be available after inheritance from EicGeoParData;
  virtual void PlaceG4Volume(G4LogicalVolume *mother, bool check = false, 
  			     void *pRot = 0, void *tlate = 0) = 0;
  virtual const std::map<G4VPhysicalVolume*, unsigned>  &GetG4SensitiveVolumes( void ) const = 0;

 protected:
  // Sets default parameter values;
  void SetDefaultParameters() override;

 private:
  // Detector construction;
  EicRootDetector* m_Detector;

  // G4 hit "name", like G4HIT_VST;
  std::string m_G4HitName;

  // Stepping function;
  PHG4SteppingAction* m_SteppingAction;
};

#endif 
