
#include <map>
#include <vector>

#include <TString.h>
#include <TColor.h>

#ifndef _ETM_DETECTOR_
#define _ETM_DETECTOR_

class EtmDetectorStack;
class G4VPhysicalVolume;
class G4LogicalVolume;
class TopoDS_Shape;

#include <EtmPalette.h> 
#include <EtmPolygonGroup.h>

// For anything but the beam line edges;
#define _DEFAULT_TRIM_VALUE_  (0.7654321)

class EtmDetector: public EtmPolygonGroup {
  friend class EicToyModel;
  friend class EtmDetectorStack;

 public:
  // Make ROOT streamer happy;
  EtmDetector(const EtmDetectorStack *stack = 0, const char *label = 0, 
	      int fillcolor = 0, int linecolor = 0, double length = 0.0);
  ~EtmDetector() {};

  // == User commands === just the shortcuts for the "interactive" ones ==========================
  //
  //virtual EtmDetector *segments(unsigned segmentation) { 
  //mSegmentation = segmentation; return this; 
  //};
  //
  // For better vizualization may want either to align the detector edges with the 
  // respective eta lines, or with the arm alignment axis; can do this for two edges separately;
  EtmDetector *trim(double etamin, double etamax, bool preserve_modified = false);
  EtmDetector *trim(double value) { return trim(value, value, false); };
  EtmDetector *brick( void ) { return trim(0.0); };
  //
  // May want to change the detector volume contour line;
  EtmDetector *dashed( void )  { SetLineStyle(etm::dashed); return this; };
  EtmDetector *dotted( void )  { SetLineStyle(etm::dotted); return this; };
  EtmDetector *text(int value) { SetTextColor(value);       return this; };
  //
  // May want to include subvolumes, recursively; this is only good for visualization purposes;
  //EtmPolygonGroup *insert(                 int fillcolor, int linecolor = kBlack);
  EtmPolygonGroup *insert(const char *tag,                int linecolor = kBlack);
  //EtmPolygonGroup *insert(const char *tag, int fillcolor, int linecolor = kBlack);

  // Returns the detector length along the alignment axis; 
  double length( void ) const { return mLength; };
  void length(double value)   { mLength = value; };

  // For bck() and fwd() stacks: absolute offset towards mid() stack, as counted from the 
  // alignment axis; perhaps relative to some other detector *upstream*side*, if specified;
  // mid() stack requires neighbor stack specification, that's why more invocations provided;
  EtmDetector *stretch(EtmDetector *refdet,        double toffset, etm::Stretch how = etm::kRecess);
  EtmDetector *stretch(EtmDetector *refdet,                        etm::Stretch how = etm::kRecess) {
    return stretch(refdet,     0.0, how);
  };
  EtmDetector *stretch(                            double tlength, etm::Stretch how = etm::kRecess);
  EtmDetector *stretch(EtmDetectorStack *refstack, double tlength, etm::Stretch how = etm::kRecess);

  EtmDetector *highlight(double alpha = 1.0) { mColorAlpha = alpha; return this; };
  // =============================================================================================

  double GetActualDistance( void ) const { return mActualDistance; };
  bool IsDummy( void )             const { return !mLabel; };

  // Well, 'world' here means some 'parent' volume, actually;
  G4VPhysicalVolume *PlaceG4Volume(G4LogicalVolume   *world, const char *name = 0);
  G4VPhysicalVolume *PlaceG4Volume(G4VPhysicalVolume *world, const char *name = 0);
  G4VPhysicalVolume *GetG4Volume( void );// const { return mG4PhysicalVolume; };

  void Export(const char *fname);

  bool IsHighlighted( void ) const;// { return true; };
  double GetColorAlpha( void ) const { return mColorAlpha; };

 private:
  TString *GetLabel( void )        const { return  mLabel; };

  void RegisterFillColor(std::map<TString, int> &legend);

  void Build( void );
  std::vector<std::pair<const TColor*, const TopoDS_Shape*> > BuildCADmodel( void );

  unsigned GetOrder( void ) const;

  void ClearPolygonBuffer( void );
  //void DrawMe( void ) const;
  //void DrawMe(unsigned tb) const;

  const EtmDetectorStack *mStack;

  TString *mLabel;

  std::vector<EtmPolygonGroup*> mGraphicsElements;

  // May want to segment the volume;
  //unsigned mSegmentation;

  // Detector length along the stack alignment axis;
  double mLength, mOffset;

  double mActualDistance;

  // Min & max eta edge alignment mode;
  double mEdgeTrimming[2];

  // A tree of possible children; no overlap control here;
  //std::vector<EtmDetector*> mChildren;

  G4VPhysicalVolume *mG4PhysicalVolume; //!

  // FIXME: use awkward ROOT teminology;
  double mColorAlpha;

  ClassDef(EtmDetector, 1)
};

#endif
