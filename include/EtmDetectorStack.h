
#include <map>
#include <vector>

#include <TString.h>

#ifndef _ETM_DETECTOR_STACK_
#define _ETM_DETECTOR_STACK_

#include <EtmEtaBoundary.h>
#include <EtmDetector.h>

class EtmDetectorStack: public TObject {
  friend class EicToyModel;
  friend class EtmAzimuthalScan;

 public:
  EtmDetectorStack(const char *label = 0, double etamin = 0.0, double etamax = 0.0);
  ~EtmDetectorStack() {};

  // == User commands === just the shortcuts for the "interactive" ones ==========================
  //
  // Add the last detector (std::vector push_back); fill the stack starting from IP;
  EtmDetector *add(const char *tag,            double length);
  EtmDetector *add(const char *tag, int color, double length);
  // Two special types of "detectors";
  EtmDetector *gap(                            double length) { return add("", length); };
  // Useful for available space scans, etc;
  EtmDetector *marker( void );
  //
  // Stick a detector *before* a specified one;
  EtmDetector *insert(const char *tag, double length, const char *after, unsigned order = 0);
  //
  // Remove the detector, either by name or by name+order;
  void rm(const char *dname);
  void rm(const char *dname, unsigned order);
  //
  // Reconfigure the stack starting distance and order; 
  void offset(double dstart = 0.0);
  //
  // Eta boundary getters;
  double etamin( void ) const { return mEtaBoundaries[0]->GetEta(); };
  double etamax( void ) const { return mEtaBoundaries[1]->GetEta(); };

  // Get detector by name & order (in case there is more than one of this type);
  EtmDetector *get(const char *dname, unsigned order = 0) const;
  // =============================================================================================

  void SetEtaBoundaries(double min, double max); 

  /*const*/ EtmEtaBoundary *EtaBoundary(unsigned mm) const { 
    return (mm <= 1 ? mEtaBoundaries[mm] : 0);
  };
  const TVector2 &AlignmentAxis( void ) const { return mAlignmentAxis; };
  const TString &GetLabel( void )       const { return mLabel; };
  unsigned DetectorCount( void )        const { return mDetectors.size(); };
  const EtmDetector *GetDetector(unsigned id) const { 
    return (id < mDetectors.size() ? mDetectors[id] : 0); 
  };

 private:
  double FullLength( void ) const { return mFullLength; };
  void CalculateAlignmentAxis( void );

  EtmDetector *CreateDetector(const char *tag, double length);

  void Build( void );
  void CalculateActualDistances( void );

  TString mLabel;

  // Booked detectors; yes, prefer a vector rather than a set, in order to preserve the order;
  // individual element removal is indeed more complicated then;
  std::vector<EtmDetector*> mDetectors;

  EtmEtaBoundary *mEtaBoundaries[2]; 
  double mStartingDistance, mFullLength;

  // Alignment axis however will be +Z for both horizontal and vertical views;
  TVector2 mAlignmentAxis;

  // Useful not to crash user codes, and also indicate situations when stretch() call 
  // was not given relative to a particular detector; do not want to pollute mDetectors[]; 
  EtmDetector *mDummyDetector; //!

  ClassDef(EtmDetectorStack, 1)
};

#endif
