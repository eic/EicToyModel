#include "EicRootDetector.h"
#include "EicRootSubsystem.h"

#include <phparameter/PHParameters.h>

#include <g4main/PHG4Subsystem.h>
#include <g4main/PHG4HitContainer.h>

#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <etm/EicToyModel.h>

#include <Geant4/G4VPhysicalVolume.hh>

using namespace std;


// ---------------------------------------------------------------------------------------

EicRootDetector::EicRootDetector(PHG4Subsystem *subsys, PHCompositeNode *Node,
				 const std::string &dnam)
  : PHG4Detector(subsys, Node, dnam)
  , m_HitContainer(0)
{
} // EicRootDetector::EicRootDetector()

// ---------------------------------------------------------------------------------------

int EicRootDetector::IsInDetector(G4VPhysicalVolume *volume) //const
{
  auto subsys = dynamic_cast<EicRootSubsystem*>(GetMySubsystem());
  auto svols = subsys->GetG4SensitiveVolumes();

  if (svols.find(volume) == svols.end()) return 0;

  // Well, this looks dumb, but I do not want to break the fun4all stepping code
  // convention where 0 return code means "out of detector"; 
  return svols[volume] + 1;
} // EicRootDetector::IsInDetector()

// ---------------------------------------------------------------------------------------

void EicRootDetector::ConstructMe(G4LogicalVolume *logicWorld)
{
  // The subsystem; it is this class, which inherits from EicGeoParData;
  auto subsys = dynamic_cast<EicRootSubsystem*>(GetMySubsystem());

  // Get pointer to the mother volume and place subdetector volumes into it;
  std::string mvolume = subsys->GetParams()->get_string_param("MotherVolume");
  if (mvolume == "DefaultParameters-Invalid")
    subsys->PlaceG4Volume(logicWorld, true, 0, new G4ThreeVector(0, 0, 0));
  else {
    // EicToyModel singleton;
    auto eic = EicToyModel::Instance();

    auto mid = eic->mid()->get(mvolume.c_str())->GetG4Volume();
    subsys->PlaceG4Volume(mid->GetLogicalVolume(), true, 0, new G4ThreeVector(0, 0, 0));
  } //if

  // Declare sensitive volumes;
  //for(auto phy: subsys->GetG4SensitiveVolumes())
  //m_PhysicalVolumesSet.insert(phy);

  return;
} // EicRootDetector::ConstructMe()

// ---------------------------------------------------------------------------------------

void EicRootDetector::Print(const std::string &what) const
{
  // FIXME: need to implement this stuff;
  cout << "EicRoot Detector:" << endl;
  if (what == "ALL" || what == "VOLUME") {
    cout << "Version 0.1" << endl;
    cout << "Parameters:" << endl;
    //m_Params->Print();
  } //if

  return;
} // EicRootDetector::Print()

// ---------------------------------------------------------------------------------------
