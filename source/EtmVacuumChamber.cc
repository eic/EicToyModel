
#include <iostream>
#include <fstream>

#include <unistd.h>

#include <TObjString.h>

#ifdef _VGM_
#include "Geant4GM/volumes/Factory.h"
#include "RootGM/volumes/Factory.h"
#endif

#ifdef _ETM2GEANT_
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SubtractionSolid.hh"
#endif

#include <EtmVacuumChamber.h>
#include <EicToyModel.h>

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

EtmVacuumChamber::EtmVacuumChamber( void ): 
  mTGeoModel(0), mHadronBeamPipeOpening(0.0), mActualCrossingAngle(0.0),
  mStandaloneMode(false), g4Factory(0)
{
} // EtmVacuumChamber::EtmVacuumChamber()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void EtmVacuumChamber::CreateMedium(const char *name, double A, double Z, double density)
{
  // FIXME: do it better;
  static unsigned counter;

  new TGeoMedium (name, ++counter, new TGeoMaterial(name, A, Z, density));
} // EtmVacuumChamber::CreateMedium()

// ---------------------------------------------------------------------------------------

void EtmVacuumChamber::CreateWorld( void )
{
  auto eic = EicToyModel::Instance();

  // Sanity checks;
  if (mTGeoModel && !mStandaloneMode) return;

  // First see whether gGeoManager is available;
  if (!mTGeoModel && gGeoManager)
    mTGeoModel = gGeoManager;
  else {
    if (mTGeoModel) delete mTGeoModel;

    // Create a standalone instance otherwise;
    mTGeoModel = new TGeoManager("VC.ROOT", "Simplified IR vacuum chamber geometry");

    // ROOT media database is rather lousy compared to GEANT; just create everything
    // from scratch by hand;
    CreateMedium(_ACCELERATOR_VACUUM_,  1.008,  1, 1E-10);
    CreateMedium(   _UNIVERSE_VACUUM_,  1.008,  1, 1E-10);
    CreateMedium(                "Be",  9.01,   4,  1.85);
    CreateMedium(                "Al", 26.98,  13,  2.70);

    // FIXME: could do this better of course;
    //printf("GetIrRegionLength(): %f\n", GetIrRegionLength());
#if 1//_OLD_
    auto world = mTGeoModel->MakeBox("World", mTGeoModel->GetMedium(_UNIVERSE_VACUUM_),
				     // FIXME: GetIrRegionLength() here returns default value
				     // rather than the one encoded in the .root dump;
				     200., 200., eic->GetIrRegionLength() + 100.);
    mTGeoModel->SetTopVolume(world); 
#else
    auto world = new TGeoVolumeAssembly("World");
    mTGeoModel->SetTopVolume(world);   
#endif
    world->SetVisibility(kFALSE);

    mStandaloneMode = true;
  } //if
} // EtmVacuumChamber::CreateWorld()

// ---------------------------------------------------------------------------------------

bool EtmVacuumChamber::CrossingAngleResetPossible(double value) const 
{
  // So: either geometry can be tuned dynamically or the requested value 
  // matches the geometry; even if flexible, only a single change is allowed 
  // in a non-standalone mode (otherwise why would one like to erase the whole 
  // geometry tree because of a change in the vacuum chamber); FIXME: may want to 
  // erase these nodes selectively later;
  return (ConfigurableCrossingAngle() || (FixedCrossingAngle() == value)) && 
    (mStandaloneMode || !mTGeoModel);
} // EtmVacuumChamber::CrossingAngleResetPossible()

// ---------------------------------------------------------------------------------------

void EtmVacuumChamber::CheckGeometry(bool force)
{
  double crossing_angle = EicToyModel::Instance()->GetCrossingAngle();
    
  //printf("EtmVacuumChamber::CheckGeometry(): %f\n", crossing_angle);
  // Rebuild only if the crossing angle changed; FIXME: it is assumed of course that 
  // EicToyModel ctor sets mCrossingAngle to 25mrad;
  if (mActualCrossingAngle != crossing_angle || force) {
    CreateWorld();
    CreateGeometry();
    
    mActualCrossingAngle = crossing_angle;
  } //if
} // EtmVacuumChamber::CheckGeometry()

// ---------------------------------------------------------------------------------------

void EtmVacuumChamber::Export(const char *fname)
{
  CheckGeometry();

  mTGeoModel->CloseGeometry();
  mTGeoModel->CheckOverlaps(0.0001);

  // FIXME: a warning otherwise;
  if (mStandaloneMode) mTGeoModel->Export(fname);
} // EtmVacuumChamber::Export()

// ---------------------------------------------------------------------------------------

void EtmVacuumChamber::StoreGDMLdump( void )
{
  CheckGeometry();
  
  if (mStandaloneMode) {
    // Create GDML file first; FIXME: pipe(), shm, tmpfs or such?; PID tag, perhaps?;
    const char *fname = "/tmp/tmp.gdml";

    mTGeoModel->Export(fname);

    // Create input stream; read the file in; write it out as a wrapper class instance, 
    // which can be accessed without ETM library, if needed (TObjString);
    {
      std::ifstream fin(fname);

      TString str;
      str.ReadFile(fin);

      TObjString ostr; ostr.SetString(str);
      // FIXME: hardcoded;
      ostr.Write("VC.GDML");
    
      // Remove the temporary file;
      unlink(fname);
    }
  } //if
} // EtmVacuumChamber::StoreGDMLdump()

// ---------------------------------------------------------------------------------------
// 
//  Assume that certain parts of the vacuum chamber design will simply move 
//  together with the IP;
//

void EtmVacuumChamber::DrawMe( void ) //const
{
  CheckGeometry();

  auto eic = EicToyModel::Instance();

  int gray = kGray+1;
  // FIXME: step hardcoded;
  double step = 5.0 * etm::cm, zcurr = -eic->GetIrRegionLength()/2, rprev[2] = {0.0, 0.0};
  double dphi = eic->GetCurrentView() == EicToyModel::kHorizontal ? 0.0 : M_PI/2;

  for( ; zcurr <= eic->GetIrRegionLength()/2; zcurr += step) {
    double rcurr[2] = {GetRadialSize(zcurr, dphi+M_PI), GetRadialSize(zcurr, dphi)};
    if (rprev[0] && rprev[1] && rcurr[0] && rcurr[1]) {
      double xx[4] = {  zcurr-step,   zcurr, zcurr, zcurr-step};
      double yy[4] = { -rprev[0], -rcurr[0], rcurr[1], rprev[1]};
      
      eic->DrawPolygon(4, xx, yy, gray, false);
    } //if
    for(unsigned du=0; du<2; du++)
      rprev[du] = rcurr[du];
  } //for inf
} // EtmVacuumChamber::DrawMe()

// ---------------------------------------------------------------------------------------

//
//  FIXME: unify with the azimuthal scan; FIXME: this assumes that the electron beam 
// pipe is aligned with Z axis, therefore point (0,0) is *inside* the vacuum 
// chamber, no matter what; 
//

double EtmVacuumChamber::GetRadialSize(double z, double phi) //const
{
  CheckGeometry();

  // Assume that the beam pipe is positioned in the World volume already (in other
  // words the IP shift is taken into account); then 'z' does not require any 
  // further shifts, obviously;
  double xx[3] = {0.0, 0.0, z};
  double nn[3] = {cos(phi), sin(phi), 0.0}, outerR = 0.0;

  mTGeoModel->SetCurrentPoint    (xx);
  mTGeoModel->SetCurrentDirection(nn);

  for(auto node = mTGeoModel->GetCurrentNode(); ; ) {
    auto material = node->GetVolume()->GetMaterial();
      
    mTGeoModel->FindNextBoundary();
    // Switch to next node along {xx, nn[]} 3D line;
    node = mTGeoModel->Step();

    // If it was NOT a vacuum volume, record the radius at the boundary;
    //if (material != mTGeoModel->GetMaterial("Vacuum")) {
    if (material != mTGeoModel->GetMaterial(_ACCELERATOR_VACUUM_) && 
	material != mTGeoModel->GetMaterial(_UNIVERSE_VACUUM_)) {
      const double *pt = mTGeoModel->GetCurrentPoint();
      outerR = sqrt(pt[0]*pt[0] + pt[1]*pt[1]);
    } //if	    
    
    //assert(model->IsEntering());
    
    // Once out of volume, break;
    if (mTGeoModel->IsOutside()) break; 
  } //for inf

  return outerR;
} // EtmVacuumChamber::GetRadialSize()

// ---------------------------------------------------------------------------------------

G4VSolid *EtmVacuumChamber::CutThisSolid(G4VSolid *solid, double dz)
{
#if defined(_ETM2GEANT_) && defined(_VGM_)
  // Do this once upon startup: switch TGeoManager pointer -> calculate vacuum chamber 
  // TGeo geometry dynamically -> convert to G4 geometry -> switch TGeoManager pointer back;
  if (!g4Factory) {
    g4Factory = new Geant4GM::Factory();

    // Switch TGeoManager pointer;
    TGeoManager *saveGeoManager = gGeoManager; gGeoManager = 0;
    CheckGeometry();
    
    RootGM::Factory rtFactory;
    rtFactory.Import(gGeoManager->GetTopNode());
    
    //++g4Factory.SetDebug(1);
    rtFactory.Export((Geant4GM::Factory*)g4Factory);
    
    // Return TGeoManager pointer back;
    delete gGeoManager; gGeoManager = saveGeoManager;
  } //if
    
  {
    G4VSolid *ptr = solid;
    G4VPhysicalVolume *gcut = ((Geant4GM::Factory*)g4Factory)->World();
    auto glog = gcut->GetLogicalVolume();
    
    for(int iq=0; iq<glog->GetNoDaughters(); iq++) {
      auto daughter = glog->GetDaughter(iq);
      
      // Hope the algebra is correct here?;
      CLHEP::Hep3Vector vv(0, 0, dz);//, uu = daughter->GetRotation() ? (*daughter->GetRotation() * vv) : vv;
      
      //ptr->DumpInfo();
      ptr = new G4SubtractionSolid(ptr->GetName(), ptr, daughter->GetLogicalVolume()->GetSolid(),
				   daughter->GetRotation(), daughter->GetTranslation() - vv); 
    } //for iq

    return ptr;
  }
#else
  return solid;
#endif
} // EtmVacuumChamber::CutThisSolid()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

ClassImp(EtmVacuumChamber)
