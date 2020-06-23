
#include <set>
#include <map>

#include <TString.h>
#include <TCanvas.h>

#ifndef _EIC_TOY_MODEL_
#define _EIC_TOY_MODEL_

class G4LogicalVolume;
class G4VPhysicalVolume;

#include <EtmPalette.h>
#include <EtmAzimuthalScan.h>
#include <EtmVacuumChamber.h>
#include <EtmDetectorStack.h>

// +/-4.5 m; up to ~4m radius suffices?; can be changed in the ctor (only);
#define _IR_REGION_LENGTH_DEFAULT_ (900.0)
#define _IR_REGION_RADIUS_DEFAULT_ (400.0)

class EicToyModel: public TObject {
  // Here and in other "main" classes: more concerned to prevent user access to the non-const
  // methods, which they are not supposed to use rather than anything else; so consider 
  // to define friend classes rather than keeping these methods public;
  friend class EtmDetector;
  friend class EtmDetectorStack;
  friend class EtmAzimuthalScan;
  friend class EtmVacuumChamber;

 public:
  EicToyModel(double length = _IR_REGION_LENGTH_DEFAULT_, double radius = _IR_REGION_RADIUS_DEFAULT_);

  // == User commands === just the shortcuts for the "interactive" ones ==========================
  //
  // -- Geometry ---------------------------------------------------------------------------------
  // Vacuum chamber derived class and eta *boundaries* can still be modified later (either 
  // interactively or after model export-import), but not the *number* of different 
  // stacks; too much of a trouble to re-create the stack structure dynamically (in other 
  // words to guarantee that the meaning of bck(), mid() & fwd() stays the same), so eta() 
  // calls beyond LockGeometry() will not work; 
  EicToyModel *acceptance(double eta0, double eta1, double eta2, double eta3, 
			  bool reset_stacks = false, bool redraw = true);
  EicToyModel *DefineVacuumChamber(EtmVacuumChamber *vc);
  //EicToyModel *DefineVacuumChamber(const char *fname);
  //
  // Nominal IP along the beam line direction; negative values correspond to the e-endcap direction;
  EicToyModel *ip(double offset,                bool redraw = true);
  EicToyModel *ir(double length, double radius, bool redraw = true);
  //
  // Access to backward endcap, mid rapidity and forward endcap stacks;
  EtmDetectorStack *vtx( void ); EtmDetectorStack *vertex  ( void ) { return vtx(); };
  EtmDetectorStack *bck( void ); EtmDetectorStack *backward( void ) { return bck(); };
  EtmDetectorStack *mid( void ); EtmDetectorStack *barrel  ( void ) { return mid(); };
  EtmDetectorStack *fwd( void ); EtmDetectorStack *forward ( void ) { return fwd(); };
  //
  // Calls describing the outline of the vacuum chamber;
  EicToyModel *SetCrossingAngle(double value, bool redraw = true);
  //
  // -- Visualization ----------------------------------------------------------------------------
  //
  // May want to set a different canvas width;
  EicToyModel *width(unsigned width);
  //
  // Enable/disable mirror image; NB: top and bottom halves will look differently in the H-section;
  EicToyModel *mirror(bool what = true, bool redraw = true);
  // Either full view or only the left (right) halves;
  EicToyModel *full (bool redraw = true) { mOneSideMode = EicToyModel::kOff;   home(redraw); return this; };
  EicToyModel *left (bool redraw = true) { mOneSideMode = EicToyModel::kLeft;  home(redraw); return this; };
  EicToyModel *right(bool redraw = true) { mOneSideMode = EicToyModel::kRight; home(redraw); return this; };
  //
  // Enable/disable the legend;
  EicToyModel *legend(bool what = true, bool redraw = true);
  //
  // Draw the view (either vertical or horizontal section);
  void vdraw( void ) { DrawMe(EicToyModel::kVertical);   }; 
  void hdraw( void ) { DrawMe(EicToyModel::kHorizontal); }; 
  void  draw( void ) { DrawMe(EicToyModel::kCurrent); }; 
  //
  // Camera home;
  void home(bool redraw = true);
  // Zoom view;
  void zoom(double blX, double blY, double trX, double trY, bool redraw = true);
  //
  // Add an extra eta line for drawing; perhaps either with no actual line or no label;
  EicToyModel *AddEtaLine(double value, bool line = true, bool label = true, bool redraw = true);
  // Remove an eta line; NB: this only removes *currently*defined* eta lines; the next 
  // acceptance() call will call 4x AddEtaLine() internally; 
  EicToyModel *RemoveEtaLine(double eta) { mEtaLines.erase(eta); return this; };
  //
  // -- Other ------------------------------------------------------------------------------------
  //
  // Save the current state and TGeo model in a .root file; 
  void write(bool everything = false, bool lock = false);
  //void Write( void )  { write(); };
  //
  // This call also defines the output files basenames; 
  EicToyModel *SetName( const char *name)        { mName = name; return this; };
  //
  // Safety clearance will affect the minimal gaps introduced between the integration volumes; 
  //EicToyModel *SafetyClearance(double value)     { mSafetyClearance = fabs(value); return this; };
  // Visual clearance will only affect the display picture;
  //EicToyModel *VisualClearance(double value)     { mVisualClearance = fabs(value); return this; };
  //
  // If the marker() objects (kind of indicating the locations of the last silicon stations, 
  // which can be achieved in this detector composition) are defined in the forward and backward 
  // endcaps, regions where the field is supposed to better be aligned with the solenoid axis will 
  // be shown;
  EicToyModel *DrawFlatFieldLines(double eta);

  void ApplyStandardTrimming( void );
  // =============================================================================================

  static EicToyModel *Instance( void ) { return mInstance; };

  // Conversion to canvas coordinates; cnv() is for less typing, sorry; 
  TVector2 cnv(const TVector2 &world)  const;

  // In world coordinates;
  TVector2 GetIpLocation( void )       const { return TVector2(mIpOffset, 0.0); };

  double GetCrossingAngle( void )      const { return mCrossingAngle; };
  double GetIrRegionLength( void )     const { return mIrRegionLength; };
  double GetIrRegionRadius( void )     const { return mIrRegionRadius; };

  static std::pair<std::vector<TVector2>, std::vector<TVector2> > 
    GetIntersection(const std::vector<TVector2> &chain, const EtmLine2D &line);

  void Construct( void )                     { DrawMe(EicToyModel::kUndefined, false); };
  // Well, 'world' here means some 'parent' volume, actually;
  void PlaceG4Volumes(G4LogicalVolume *world);
  void PlaceG4Volumes(G4VPhysicalVolume *world);
  void Export(const char *fname, bool everything = false, bool lock = false);
  void ExportVacuumChamber(const char *fname = 0);
  G4VPhysicalVolume *ConstructG4World( void );

  static EicToyModel *Import(const char *fname);
  // There is no good reason to Get() the TGeoManager instance sitting in the same 
  // .root file -> re-create it from scratch;
  void BuildVacuumChamber( void ) { if (mVacuumChamber) mVacuumChamber->CheckGeometry(true); };

  void SetAzimuthalSegmentation(unsigned value)   { 
    if (!mGeometryLocked) mAzimuthalSegmentation = value; 
  };
  unsigned GetAzimuthalSegmentation( void ) const { return mAzimuthalSegmentation; };
  
  //const TString &GetName( void ) const { return mName; };
  //const char *GetName( void )    const { return mName.Data(); };

 private:
  ~EicToyModel() {}; 

  enum OneSide { kOff, kLeft, kRight};
  enum View {kUndefined, kCurrent, kHorizontal, kVertical};

  EicToyModel::View GetCurrentView( void ) const { return mCurrentView; };

  void DrawPolygon(unsigned dim, const double xx[], const double yy[], 
		   int color, bool line = true) const;

  EtmPalette &Palette( void )                { return mPalette; };

  EtmVacuumChamber *GetVacuumChamber( void )   { return mVacuumChamber; };

  int ExportCADmodel(const char *fname);

  //double SafetyClearance( void )       const { return mSafetyClearance; };
  //double VisualClearance( void )       const { return mVisualClearance; };

  const char *GetName( void ) const { return mName.Data(); };

  void CalculateDefaultEtaBoundaries(EtmDetectorStack *single = 0);
  std::vector<TVector2> RebuildCrack(const std::vector<TVector2> &chain, 
				     const EtmLine2D &am, const EtmLine2D &ap,
				     const EtmLine2D &cut, etm::Stretch how);
  void AppendChain(std::vector<TVector2> &chain, const std::vector<TVector2> &add);

  std::vector<TVector2> Band(const std::vector<TVector2> &chain, 
			     const EtmLine2D &from, const EtmLine2D &to);

  TVector2 CalculateEtaStartPoint(double eta, unsigned vh, unsigned tb, 
				  bool account_crossing_angle);
  std::pair<TVector2, bool> CalculateEtaEndPoint(double eta, unsigned vh, unsigned tb, 
						 bool account_crossing_angle);

  // Access to a stack by eta;
  EtmDetectorStack *GetStack(double eta);

  void WriteText(const TVector2 &where, const TString &what, int color = kBlack) const;

  void DrawSingleLine(const TVector2 &from, const TVector2 &to, int color = kBlack, 
		      int width = 1, etm::LineStyle style = etm::solid) const;

  EicToyModel *Enable(const char *tag) { mEnabledColorTags.insert(tag); return this; };

  EicToyModel *RebuildEverything(bool redraw);

  // It looks impractical to have EtmDetector::DrawMe() like classes, since 
  // this introduces backdoor dependencies; so just do all the loops in this single call;
  void DrawMe(EicToyModel::View view = EicToyModel::kCurrent, bool draw = true);

  // Two ends and a couple of attributes;
  void DrawSingleEtaLine(double eta, const TVector2 &to, std::pair<bool, bool> what, bool side) const;

  TVector2 GetCanvasCoordinates(const TVector2 &world) const { return cnv(world); };

  // FIXME: remove xx[],yy[] -> TVector2 re-shuffling later;
  void DrawRectangle(const TVector2 &bl, const TVector2 &tr, int color, bool line = true) const;

  void DrawEtaLines( void );
  void DrawEtaBoundaries( void );
  void DrawBeamLine( void );
  void DrawAxisTicks( void );
  void DrawColorLegend(unsigned rows = 1);
  void DrawMarkers( void );

  void DrawFlatFieldLines( void );

  void AddBoundaryModificationRequest(EtmDetector *hostdet, EtmDetector *refdet, 
				      double toffset, etm::Stretch how);
  void ExecuteBoundaryModificationRequests(EtmDetectorStack *single = 0);
  void PurgeBoundaryVertexArrays( void );
  void AccountServiceMaterials( void );

  static EicToyModel *mInstance;

  // Want this to be a vector, just in case far forward part is added later; 
  std::vector<EtmDetectorStack*> mStacks;

  TString mName;

  double mIrRegionLength, mIrRegionRadius;

  unsigned mAzimuthalSegmentation;

  // Nominal IP offset; in order to avoid any further complications, certain assumptions are made 
  // about the coordinate system and axis orietation:
  //
  //   - Z-axis of the coordinate system is aligned with the outgoing electron beam (Z- direction);
  //   - crossing angle is only considered in the horizontal plane (even if later on a small Y-slope 
  //     due to the solenoid compensation scheme will become needed);
  //   - pseudo-rapidities are counted with respect to the outgoing electron direction 
  //     for negative values, and with respect to the outgoing hadron direction for positive values;
  //     this means that the h-endcap ranges will be rotated by 25mrad in the horizontal projection;
  //   - endcap detector integration volumes will be orthogonal to Z- direction for both arms (nobody 
  //     knows yet, how they will be installed in reality, and h-endcap polygons will look ugly 
  //     if aligned at 25mrad);
  //   - IP offset is only considered along Z-axis; X=Y=0 per definition; if this ever becomes a problem, 
  //     it is easier to offset the vacuum chamber;
  //   - Z=0 is the center of the "nominal" +/-4.5m region;
  //   - exported integration volumes will be axially symmetric with respect to the Z-axis TGeoPolycon 
  //     objects, perhaps at a non-zero X-offset, with a hole matching the layout of the vacuum chamber; 
  //     the shape will be taken as a "boolean &" of the phi=0,90,180,270 degree projections with respect 
  //     to the guessed "center" (X-offset);
  double mIpOffset;

  // XY-size of the canvas;
  unsigned mXdim, mYdim;
  // XY-scale and offsets used in GetCanvasCoordinates();
  double mXsize, mYsize, mX0, mY0;

  bool mColorLegendEnabled, mZoomedView;

  // A set of boundary and user-defined eta lines to be drawn; <eta, <line,label>>;
  std::map<double, std::pair<bool, bool> > mEtaLines;

  TCanvas *mCanvas; //!

  EtmVacuumChamber *mVacuumChamber;

  EicToyModel::View mCurrentView;

  bool mMirrorImage;
  EicToyModel::OneSide mOneSideMode;

  EtmPalette mPalette;

  std::set<TString> mEnabledColorTags;

  // The first one will be used to generate mBoundaries[] array and the 
  // integration volumes; the second one is a guaranteed gap between 
  // objects in the TCanvas representation; 
  //double mSafetyClearance, mVisualClearance;

  double mCrossingAngle;

  std::set<double> mFlatFieldLinesEta;

  std::vector<EtmBoundaryModificationRequest*> mBoundaryModificationRequests;

  // A naive way to lock the geometry, so that the .root file can be imported, and 
  // even the commands changing the internal structure will seemingly work, but 1) none 
  // of the methods, which actually modify the "final" geometry dynamically will have 
  // any effect, 2) for the sake of completeness, write() call will be disabled; this 
  // call is intentionally "irreversible"; does not prevent a smart user from writing 
  // UnlockGeometry() method of course, but un-intentional changes can not happen;
  bool mGeometryLocked;

  bool mNewCanvasRequired; //!

  ClassDef(EicToyModel, 1)
};

#endif
