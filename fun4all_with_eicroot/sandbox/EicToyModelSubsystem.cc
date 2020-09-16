//____________________________________________________________________________..
//
// This is the interface to the framework. You only need to define the parameters
// you use for your detector in the SetDefaultParameters() method here
// The place to do this is marked by //implement your own here//
// The parameters have no units, they need to be converted in the
// EicToyModelDetector::ConstructMe() method
// but the convention is as mentioned cm and deg
//____________________________________________________________________________..
//
#include "EicToyModelSubsystem.h"
#include "EicToyModelDetector.h"

using namespace std;

//_______________________________________________________________________
EicToyModelSubsystem::EicToyModelSubsystem(const std::string &name)
  : PHG4DetectorSubsystem(name)
  , m_Detector(nullptr)
{
  // call base class method which will set up parameter infrastructure
  // and call our SetDefaultParameters() method
  InitializeParameters();
}

//_______________________________________________________________________
int EicToyModelSubsystem::InitRunSubsystem(PHCompositeNode *topNode)
{
  // create detector
  m_Detector = new EicToyModelDetector(this, topNode, GetParams(), Name());
  m_Detector->OverlapCheck(CheckOverlap());
  m_Detector->SuperDetector(SuperDetector());

  return 0;
}

//_______________________________________________________________________
void EicToyModelSubsystem::Print(const string &what) const
{
  if (m_Detector)
  {
    m_Detector->Print(what);
  }
  return;
}

//_______________________________________________________________________
PHG4Detector *EicToyModelSubsystem::GetDetector(void) const
{
  return m_Detector;
}

//_______________________________________________________________________
void EicToyModelSubsystem::SetDefaultParameters()
{
  // sizes are in cm
  // angles are in deg
  // units should be converted to G4 units when used
  //implement your own here//
  set_default_double_param("place_x", 0.);
  set_default_double_param("place_y", 0.);
  set_default_double_param("place_z", 0.);
  set_default_double_param("rot_x", 0.);
  set_default_double_param("rot_y", 0.);
  set_default_double_param("rot_z", 0.);
  set_default_double_param("size_x", 20.);
  set_default_double_param("size_y", 20.);
  set_default_double_param("size_z", 20.);

  set_default_string_param("EtmInputRootFile", "DefaultParameters-Invalid");
}
