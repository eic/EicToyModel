//____________________________________________________________________________..
//
// This is the interface to the framework. You only need to define the parameters
// you use for your detector in the SetDefaultParameters() method here;
//
// The parameters have no units, they need to be converted in the ConstructMe() method
//____________________________________________________________________________..
//

#include "EicRootSubsystem.h"
#include "EicRootDetector.h"
#include "EicRootSteppingAction.h"

#include <phparameter/PHParameters.h>

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4SteppingAction.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>

using namespace std;

// ---------------------------------------------------------------------------------------

EicRootSubsystem::EicRootSubsystem(const std::string &name)
  : PHG4DetectorSubsystem(name)
  , m_Detector(nullptr)
  , m_SteppingAction(nullptr)
{
  // Avoid all the complications with "G4HIT_VST_[i]" hit collection names, etc;
  // as long as one can distinguish the layers via detector ID encoded in the
  // hit class instance in the stepping call, this only gives one a performance
  // gain (potentially), but forces one to play dirty games trying to assign 
  // layer resolutions based on the index ID, etc; just create more than one 
  // instance of this subsystem if e.g. spatial resolution of the inner and 
  // outer silicon layer is supposed to be different (and pass it as such 
  // to the Kalman filter);
  SuperDetector(name);

  m_G4HitName = "G4HIT_" + name;

  // Call the base class method which will set up parameter infrastructure
  // and call our SetDefaultParameters() method internally;
  InitializeParameters();
} // EicRootSubsystem::EicRootSubsystem()

// ---------------------------------------------------------------------------------------

int EicRootSubsystem::InitRunSubsystem(PHCompositeNode *topNode)
{
  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));

  // Create detector;
  m_Detector = new EicRootDetector(this, topNode, Name());
  m_Detector->OverlapCheck(CheckOverlap());

  // fun4all black magic around G4 hit container creation;
  if (GetParams()->get_int_param("active")) {
    auto DetNode = new PHCompositeNode(SuperDetector());

    dstNode->addNode(DetNode);

    m_Detector->set_hitcontainer(new PHG4HitContainer(GetG4HitName()));
    DetNode->addNode(new PHIODataNode<PHObject>(m_Detector->get_hitcontainer(), GetG4HitName(), "PHObject"));
    m_SteppingAction = new GdmlImportDetectorSteppingAction(m_Detector, GetParams());
  } //if

  return 0;
} // EicRootSubsystem::InitRunSubsystem()

// ---------------------------------------------------------------------------------------

int EicRootSubsystem::process_event(PHCompositeNode *topNode)
{
  // pass top node to stepping action so that it gets
  // relevant nodes needed internally
  if (m_SteppingAction) m_SteppingAction->SetInterfacePointers(topNode);

  return 0;
} // EicRootSubsystem::process_event()

// ---------------------------------------------------------------------------------------

void EicRootSubsystem::Print(const string &what) const
{
  if (m_Detector) m_Detector->Print(what);
} // EicRootSubsystem::Print()

// ---------------------------------------------------------------------------------------

PHG4Detector *EicRootSubsystem::GetDetector( void ) const
{
  return m_Detector;
} // EicRootSubsystem::GetDetector()

// ---------------------------------------------------------------------------------------

void EicRootSubsystem::SetDefaultParameters()
{
  // sizes are in cm
  // angles are in deg
  // units should be converted to G4 units when used
  set_default_double_param("place_x", 0.);
  set_default_double_param("place_y", 0.);
  set_default_double_param("place_z", 0.);
  set_default_double_param("rot_x",   0.);
  set_default_double_param("rot_y",   0.);
  set_default_double_param("rot_z",   0.);

  // One custom parameter;
  set_default_string_param("MotherVolume",     "DefaultParameters-Invalid");
} // EicRootSubsystem::SetDefaultParameters()

// ---------------------------------------------------------------------------------------
