// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef EICROOTSTEPPINGACTION_H
#define EICROOTSTEPPINGACTION_H

#include <g4main/PHG4SteppingAction.h>

class EicRootDetector;

class G4Step;
class G4VPhysicalVolume;
class PHCompositeNode;
class PHG4Hit;
class PHG4HitContainer;
class PHParameters;

class EicRootSteppingAction : public PHG4SteppingAction
{
 public:
  //! constructor
  EicRootSteppingAction(EicRootDetector *, const PHParameters* parameters);

  //! destructor
  virtual ~EicRootSteppingAction();

  //! stepping action
  virtual bool UserSteppingAction(const G4Step*, bool);

 private:
  //! pointer to the detector
  EicRootDetector* m_Detector;
  const PHParameters* m_Params;
  //! pointer to hit container
  PHG4HitContainer* m_HitContainer;
  PHG4HitContainer* m_AbsorberHitContainer;
  PHG4Hit* m_Hit;
  PHG4HitContainer* m_SaveHitContainer;
  G4VPhysicalVolume* m_SaveVolPre;
  G4VPhysicalVolume* m_SaveVolPost;

  int m_SaveTrackId;
  int m_SavePreStepStatus;
  int m_SavePostStepStatus;
  int m_BlackHoleFlag;
  double m_EdepSum;
};

#endif  // EICROOTSTEPPINGACTION_H
