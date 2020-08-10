
#include <phparameter/PHParameters.h>
#include <g4main/PHG4Subsystem.h>

using namespace std;

#include <EicToyModel.h>
#include "EicVstDetector.h"
#include "EicVstSubsystem.h"

//____________________________________________________________________________..
EicVstDetector::EicVstDetector(PHG4Subsystem *subsys,
                                         PHCompositeNode *Node,
                                         PHParameters *parameters,
                                         const std::string &dnam)
  : PHG4Detector(subsys, Node, dnam)
  , m_Params(parameters)
{
}

//_______________________________________________________________
int EicVstDetector::IsInDetector(G4VPhysicalVolume *volume) const
{
  set<G4VPhysicalVolume *>::const_iterator iter = m_PhysicalVolumesSet.find(volume);
  if (iter != m_PhysicalVolumesSet.end())
  {
    return 1;
  }
  return 0;
}

//_______________________________________________________________
void EicVstDetector::ConstructMe(G4LogicalVolume *logicWorld)
{
  // The subsystem; it is this class, which inherits from VstGeoParData;
  auto subsys = dynamic_cast<EicVstSubsystem*>(GetMySubsystem());

  // EicToyModel singleton;
  auto eic = EicToyModel::Instance();

  // Get pointer to the mother volume and place subdetector volumes into it;
  auto mid = eic->mid()->get(m_Params->get_string_param("MotherVolume").c_str())->GetG4Volume();
  subsys->PlaceG4Volume(mid, true, 0, new G4ThreeVector(0, 0, 0));

  // Declare sensitive volumes;
  for(auto phy: subsys->GetG4SensitiveVolumes())
    m_PhysicalVolumesSet.insert(phy);

  return;
}

//_______________________________________________________________
void EicVstDetector::Print(const std::string &what) const
{
  cout << "EicVst Detector:" << endl;
  if (what == "ALL" || what == "VOLUME")
  {
    cout << "Version 0.1" << endl;
    cout << "Parameters:" << endl;
    m_Params->Print();
  }
  return;
}
