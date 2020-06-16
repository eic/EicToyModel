
#include <EtmVacuumChamber.h>
#include <EicToyModel.h>

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

EtmVacuumChamber::EtmVacuumChamber( void ): 
  mTGeoModel(0), mHadronBeamPipeOpening(0.0), mActualCrossingAngle(0.0),
  mStandaloneMode(false)
{
} // EtmVacuumChamber::EtmVacuumChamber()

// ---------------------------------------------------------------------------------------
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
    mTGeoModel = new TGeoManager("IR", "Simplified IR vacuum chamber geometry");

    // Vacuum (low density hydrogen), beryllium, aluminum; nothing else is needed;
    auto matVacuum = new TGeoMaterial("Vacuum", 1.008,  1, 1E-10);
    auto matBe     = new TGeoMaterial("Be",     9.01,   4, 1.85);
    auto matAl     = new TGeoMaterial("Al",    26.98,  13, 2.70);
    // Fake material; want to mark particles, which do not leave the vacuum chamber;
    auto matEdge   = new TGeoMaterial("Edge",   1.0,    1, 1E-10);
    
    new TGeoMedium  ("Vacuum", 1, matVacuum);
    new TGeoMedium  ("Be",     2, matBe);
    new TGeoMedium  ("Al",     3, matAl);
    new TGeoMedium  ("Edge",   4, matEdge);
    
    // FIXME: could do this better of course;
    //printf("GetIrRegionLength(): %f\n", GetIrRegionLength());
    auto world = mTGeoModel->MakeBox("World", mTGeoModel->GetMedium("Vacuum"), 
				     // FIXME: GetIrRegionLength() here returns default value
				     // rather than the one encoded in the .root dump;
				     200., 200., eic->GetIrRegionLength() + 100.);
    mTGeoModel->SetTopVolume(world);   

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
    
  //printf("%f\n", crossing_angle);
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
    if (material != mTGeoModel->GetMaterial("Vacuum")) {
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
// ---------------------------------------------------------------------------------------

ClassImp(EtmVacuumChamber)
