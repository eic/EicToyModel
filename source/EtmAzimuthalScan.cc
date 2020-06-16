
#include <assert.h>

#include <TRandom.h>
#include <TFile.h>

#include <EicToyModel.h>
#include <EtmAzimuthalScan.h>

#ifdef _BFIELD_
#include <BeastMagneticField.h>
#endif

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

EtmAzimuthalScan::EtmAzimuthalScan(double etamin, double etamax, unsigned ntheta, unsigned nphi):
  mEtaMin(etamin), mEtaMax(etamax), mVertexSigma(0.0), mBfield(0.0), mBfieldScanStep(1 * etm::cm),
  mThetaBinCount(ntheta), mPhiBinCount(nphi), mStat(100), mBeastMagneticField(0)
{
  if (!mThetaBinCount) mThetaBinCount = 1;
  if (!mPhiBinCount)   mPhiBinCount   = 1;
} // EtmAzimuthalScan::EtmAzimuthalScan()

// ---------------------------------------------------------------------------------------

//
//  These two will cancel each other;
//

EtmAzimuthalScan *EtmAzimuthalScan::SetBfield(double field)
{ 
  mBfield = field;    

#ifdef _BFIELD_
  delete mBeastMagneticField;
  mBeastMagneticField = 0;
#endif
  
  return this; 
} // EtmAzimuthalScan::SetBfield()

EtmAzimuthalScan *EtmAzimuthalScan::SetBfield(const char *fname)
{
  // Otherwise do nothing;
#ifdef _BFIELD_
  mBeastMagneticField = new BeastMagneticField(fname);
  // Turn linear interpolation on;
  if (mBeastMagneticField->ValidMapImported()) {
    mBeastMagneticField->UseInterpolation();

    mBfield = 0.0;
  } //if
#endif

  return this;
} // EtmAzimuthalScan::SetBfield()

// ---------------------------------------------------------------------------------------

TVector3 EtmAzimuthalScan::GetBfield(const TVector3 &xx) const
{
#ifdef _BFIELD_
  if (mBeastMagneticField && mBeastMagneticField->ValidMapImported()) {
    double bx, by, bz;
    bool ok = mBeastMagneticField->GetFieldValue(xx[0], xx[1], xx[2], bx, by, bz);
    if (ok) return TVector3(bx, by, bz);

    return TVector3(0,0,0);
  } //if
#endif
  
  // Last resort: constant field;
  return TVector3(0.0, 0.0, mBfield);
} // EtmAzimuthalScan::GetBfield()

// ---------------------------------------------------------------------------------------

void EtmAzimuthalScan::DoIt(const char *fout)
{
  auto eic = EicToyModel::Instance();
  auto model = eic->GetVacuumChamber()->GetWorld();

  // So yes, for now assume only endcaps are of interest;
  assert(mEtaMin*mEtaMax > 0.0);
  auto stack = mEtaMin > 0.0 ? eic->fwd() : eic->bck();
  auto marker = stack->get(_MARKER_); if (marker->IsDummy()) return;

  bool eside = mEtaMin < 0.0;
  double sign = (eside ? -1 : 1);
  // The global location in the IR;
  double zmarker = marker ? sign*marker->GetActualDistance() + eic->GetIpLocation().X() : 0.0;

  double crossing = eside ? 0.0 : eic->GetCrossingAngle();
  double thetamin = Eta2Theta(fabs(mEtaMin > 0.0 ? mEtaMax : mEtaMin));
  double thetamax = Eta2Theta(fabs(mEtaMin > 0.0 ? mEtaMin : mEtaMax));
  double thetastep = (thetamax - thetamin)/mThetaBinCount;
  double phistep = 2*M_PI/mPhiBinCount;

  // [eta, phi]; FIXME: memory leak;
  mRL  = new TH2D("RL",  "RL",  mPhiBinCount, -M_PI, M_PI, mThetaBinCount, thetamin*1000, thetamax*1000);
  mZL  = new TH2D("ZL",  "ZL",  mPhiBinCount, -M_PI, M_PI, mThetaBinCount, thetamin*1000, thetamax*1000);
  mBdl = new TH2D("Bdl", "Bdl", mPhiBinCount, -M_PI, M_PI, mThetaBinCount, thetamin*1000, thetamax*1000);

  for(unsigned itheta=0; itheta<mThetaBinCount; itheta++) {
    double theta = thetamin + (itheta+0.5)*thetastep;

    for(unsigned iphi=0; iphi<mPhiBinCount; iphi++) {
      double phi = -M_PI + (iphi+0.5)*phistep;

      // FIXME: this should be written better;
      double nn[3] = {sin(theta)*cos(phi), sin(theta)*sin(phi), sign*cos(theta)};
      if (!eside) {
	// '-' sign here: want to "look along the outgoing beam line" for both 
	// electron and hadron cases;
	double qnn[3] = {-sin(theta)*cos(phi), sin(theta)*sin(phi), sign*cos(theta)};
	// By hand, the easiest;
	nn[0] =  qnn[0]*cos(crossing) + qnn[2]*sin(crossing);
	nn[1] =  qnn[1];
	nn[2] = -qnn[0]*sin(crossing) + qnn[2]*cos(crossing);
      } //if

      double accu = 0.0, thicku = 0.0, zair = 0.0, bdl = 0.0;

      for(unsigned ev=0; ev<mStat; ev++) {
	TVector3 curr;
	double xx[3] = {0.0, 0.0, eic->GetIpLocation().X() + gRandom->Gaus(0.0, mVertexSigma)};

	model->SetCurrentPoint    (xx);
	model->SetCurrentDirection(nn);
	    
	for(auto node = model->GetCurrentNode(); ; ) {
	  auto material = node->GetVolume()->GetMaterial();
	  
	  model->FindNextBoundary();
	  double thickness = model->GetStep();
	  double radlen = material->GetRadLen();
	  if (material != model->GetMaterial("Vacuum")) {
	    thicku += thickness;
	    accu   += thickness / radlen;
	  } //if	    
	  // Switch to next node along {xx, nn[]} 3D line;
	  node = model->Step();

	  if (material != model->GetMaterial("Vacuum"))
	    curr = model->GetCurrentPoint();

	  //assert(model->IsEntering());
	  
	  // Once out of volume, break;
	  if (model->IsOutside()) break; 
	} //for inf
	
	zair += curr[2];

	// Magnetic field scan;
	if (marker) {
	  double btsum = 0.0;
	  TVector3 x0 = curr, n0(nn);

	  for(unsigned ist=0; ; ist++) {
	    TVector3 pt = x0 + ist*mBfieldScanStep*n0;
	    if (fabs(pt[2]) > fabs(zmarker)) break;

	    TVector3 B = GetBfield(pt);
	    double scal = n0*B;
	    TVector3 Bt = B - scal*n0;
	    
	    btsum += mBfieldScanStep*Bt.Mag();
	  } //for inf

	  bdl += btsum;
	}
      } //for ev

      accu /= mStat; thicku /= mStat; zair /= mStat; bdl /= mStat;

      if (marker) {
	double zbudget = sign*(zmarker - zair); if (zbudget < 0.0) zbudget = 0.0;

	mZL ->SetBinContent(iphi+1, itheta+1, zbudget); 
	mBdl->SetBinContent(iphi+1, itheta+1, bdl/100);
      } //if

      mRL->SetBinContent(iphi+1, itheta+1, accu*100); 
    } //for iphi
  } //for itheta

  {
    auto hfile = new TFile(fout, "RECREATE"); 
    
    mRL->Write(); mZL->Write(); mBdl->Write();
    
    hfile->Close();
  }
} // EtmAzimuthalScan::DoIt()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

ClassImp(EtmAzimuthalScan)
