
#include "EicToyModelDetector.h"

#include <phparameter/PHParameters.h>

#include <etm/EicToyModel.h>

using namespace std;

//____________________________________________________________________________..
EicToyModelDetector::EicToyModelDetector(PHG4Subsystem *subsys,
                                         PHCompositeNode *Node,
                                         PHParameters *parameters,
                                         const std::string &dnam)
  : PHG4Detector(subsys, Node, dnam)
  , m_Params(parameters)
{
}

//_______________________________________________________________
int EicToyModelDetector::IsInDetector(G4VPhysicalVolume *volume) const
{
  set<G4VPhysicalVolume *>::const_iterator iter = m_PhysicalVolumesSet.find(volume);
  if (iter != m_PhysicalVolumesSet.end())
  {
    return 1;
  }
  return 0;
}

//_______________________________________________________________
void EicToyModelDetector::ConstructMe(G4LogicalVolume *logicWorld)
{
  auto eic = EicToyModel::Import(m_Params->get_string_param("EtmInputRootFile").c_str());
  // Build internal representation;
  eic->Construct();
  
  // Place all of the integration volumes into the provided World G4LogicalVolume;
  // this is optional (can also place them one by one, by hand);
  //eic->PlaceG4Volumes(logicWorld);
  eic->DefineG4World(logicWorld);

  return;
}

//_______________________________________________________________
void EicToyModelDetector::Print(const std::string &what) const
{
  cout << "EicToyModel Detector:" << endl;
  if (what == "ALL" || what == "VOLUME")
  {
    cout << "Version 0.1" << endl;
    cout << "Parameters:" << endl;
    m_Params->Print();
  }
  return;
}
