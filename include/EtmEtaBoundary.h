
#include <vector>

#include <EtmOrphans.h>
#include <EtmLine2D.h>

#ifndef _ETM_ETA_BOUNDARY_
#define _ETM_ETA_BOUNDARY_

class EtmDetector;

struct EtmBoundaryModificationRequest: public TObject {
 EtmBoundaryModificationRequest(EtmDetector *hostdet = 0, EtmDetector *refdet = 0, 
				double toffset = 0.0, etm::Stretch how = etm::kRecess): 
  mHostDetector(hostdet), mReferenceDetector(refdet), mTransverseOffset(toffset), mStretch(how) {};
  ~EtmBoundaryModificationRequest() {};

  EtmDetector *mHostDetector, *mReferenceDetector;
  double mTransverseOffset;
  etm::Stretch mStretch;

  ClassDef(EtmBoundaryModificationRequest, 1)
};

class EtmEtaBoundary: public TObject {
 public:
 EtmEtaBoundary(double eta = 0.0): mEta(eta) {};
  ~EtmEtaBoundary() {};

  void SetEta(double eta)     { mEta = eta; };
  double GetEta( void ) const { return mEta; };

  //bool mLabelRequired;

  // '[2]': [V/H]; assume that crack widths are the same for [B/T]; V- and H-values
  // can be different because of the optional rectangular section beam pipe;
  //+++std::vector<double> mCrackWidths[2];
  // '[2][2][2]': [V/H][B/T][Min/Max]; std::vector size is 'width vector + 1, obviously;
  std::vector<TVector2> mCrackNodes[2][2];

 private:
  double mEta;

  ClassDef(EtmEtaBoundary, 1);
};

#endif
