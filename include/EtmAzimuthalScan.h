
#include <TH2D.h>
#include <TVector3.h>

#ifndef _ETM_AZIMUTHAL_SCAN_
#define _ETM_AZIMUTHAL_SCAN_

class BeastMagneticField;

class EtmAzimuthalScan: public TObject {
 public:
  // FIXME: default binning hardcoded; ok;
  EtmAzimuthalScan(double etamin = 0.0, double etamax = 0.0, 
		   unsigned ntheta = 25, unsigned nphi = 120);
  ~EtmAzimuthalScan() {};

  EtmAzimuthalScan *SetVertexSigma(double sigma) { mVertexSigma = sigma; return this; };
  EtmAzimuthalScan *SetStat(unsigned stat)       { mStat = stat;         return this; };

  EtmAzimuthalScan *SetBfield(double field);
  EtmAzimuthalScan *SetBfield(const char *fname);
  EtmAzimuthalScan *SetBfieldScanStep(double step) { mBfieldScanStep = step; return this; };

  void DoIt(const char *fout);

 private:
  TVector3 GetBfield(const TVector3 &xx) const;

  // Radiation length in [%], length available for Si tracker in [cm], 
  // |Bt|*dl integral over this length in [T*m]; 
  TH2D *mRL, *mZL, *mBdl;

  double mEtaMin, mEtaMax, mVertexSigma, mBfield, mBfieldScanStep;
  unsigned mThetaBinCount, mPhiBinCount, mStat;

  BeastMagneticField *mBeastMagneticField; //!

  ClassDef(EtmAzimuthalScan, 1)
};

#endif
