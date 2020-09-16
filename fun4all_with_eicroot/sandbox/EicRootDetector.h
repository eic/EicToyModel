// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef _EICROOT_DETECTOR_
#define _EICROOT_DETECTOR_

#include <g4main/PHG4Detector.h>

#include <set>
#include <string> 

class G4LogicalVolume;
class G4VPhysicalVolume;
class PHCompositeNode;
class PHG4Subsystem;
class PHParameters;
class PHG4HitContainer;

class EicRootDetector : public PHG4Detector
{
 public:
  // ctor & dtor;
  EicRootDetector(PHG4Subsystem *subsys, PHCompositeNode *Node, const std::string &dnam);
  virtual ~EicRootDetector() {};

  // The actual geometry construction call;
  void ConstructMe(G4LogicalVolume *world) override;

  void Print(const std::string &what = "ALL") const override;

  int IsInDetector(G4VPhysicalVolume *);// const;

  PHG4HitContainer *get_hitcontainer( void ) { return m_HitContainer; };
  void set_hitcontainer(PHG4HitContainer *container) { m_HitContainer = container; };

 private:
  // active volumes
  //std::set<G4VPhysicalVolume *> m_PhysicalVolumesSet;

  PHG4HitContainer *m_HitContainer;
};

#endif  
