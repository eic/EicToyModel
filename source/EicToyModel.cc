
#include <iostream>
#include <fstream>

#include <unistd.h>

#include <TObjString.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TText.h>
#include <TFile.h>
#include <TROOT.h>
#include <TArrow.h>
#include <TPolyLine.h>

#ifdef _ETM2GEANT_
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#endif

#include <EicToyModel.h>

// Side, top & bottom extra space for labels and such;
#define _IR_REGION_SMARGIN_                   ( 40.0 * etm::cm)
#define _IR_REGION_BMARGIN_                   ( 30.0 * etm::cm)

#define _CANVAS_WIDTH_DEFAULT_                ( 1500)

// FIXME: this is dubm, but I did not find a way to calculate text width in pixels
// dynamically; somehow TText routines do not work for me;
#define _COLOR_LEGEND_HEIGHT_                  (20.0 * etm::cm)
#define _COLOR_LEGEND_STEP_                    (70.0 * etm::cm)
#define _COLOR_LEGEND_GAP_                     ( 5.0 * etm::cm)

// Use STAR drawings with Mark additions as shown at Temple in March 2020;
#define _HALL_RADIAL_BOUNDARY_                (350.0 * etm::cm)

EicToyModel *EicToyModel::mInstance = 0;

// Prefer zero default values;
#define _SAFETY_CLEARANCE_DEFAULT_              (0.0)
#define _VISUAL_CLEARANCE_DEFAULT_              (0.0)

#define _CROSSING_ANGLE_DEFAULT_               ( 0.025)

#ifdef _OPENCASCADE_
#define Printf Printf_opencascade
#include <gp_Pnt.hxx>
#include <TopoDS_Solid.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <Quantity_Color.hxx>
#include <NCollection_Vector.hxx>
#include <STEPCAFControl_Writer.hxx>

#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#undef Printf
#endif

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

EicToyModel::EicToyModel(double length, double radius): 
  mName("EicToyModel"),
  mIrRegionLength(length), 
  mIrRegionRadius(radius), mAzimuthalSegmentation(0), mIpOffset(0.0), 
  mXdim(_CANVAS_WIDTH_DEFAULT_), mYdim(0),
  mXsize(0.0), mYsize(0.0), mX0(0.0), mY0(0.0), 
  mColorLegendEnabled(true), mZoomedView(false), mCanvas(0), 
  mVacuumChamber(0),
  mCurrentView(EicToyModel::kUndefined), 
  mMirrorImage(false), mOneSideMode(EicToyModel::kOff),
  //mSafetyClearance(_SAFETY_CLEARANCE_DEFAULT_), mVisualClearance(_VISUAL_CLEARANCE_DEFAULT_),
  mCrossingAngle(_CROSSING_ANGLE_DEFAULT_), mGeometryLocked(false), mNewCanvasRequired(true)
{
  // Sanity check;
  if (mInstance) {
    printf("\n\n  EicToyModel::EicToyModel() -> Singleton instance already exists!\n\n");
    return;
  } //if

  // FIXME: control duplicate invocations;
  mInstance = this;

  // FIXME: should this be executed in a default ctor?; FIXME: hardcoded is OK?;
  acceptance(-4.0, -1.0, 1.0, 4.0, true);
} // EicToyModel::EicToyModel()

// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::Import(const char *fname)
{
  // Sanity check;
  if (mInstance) {
    printf("\n\n  EicToyModel::Import() -> Singleton instance already exists!\n\n");
    return 0;//-1;
  } //if

  // Import the ROOT file with an "EicToyModel" singleton class instance; 
  {
    TFile fin(fname);

    if (!dynamic_cast<EicToyModel *>(fin.Get("EicToyModel"))) {
      printf("Wrong file format (%s): no EicToyModel instance found!\n\n\n", fname);
      return 0;//-1;
    } //if
    
    //mVacuumChamber->mTGeoModel = 0;

    fin.Close();
  }

  return mInstance;//0;
} // EicToyModel:Import()

// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::DefineVacuumChamber(EtmVacuumChamber *vc) 
{ 
  // A bit of consistency checks;
  if (vc->CrossingAngleResetPossible(mCrossingAngle)) mVacuumChamber = vc; 

  return this; 
} // EicToyModel::DefineVacuumChamber()

// ---------------------------------------------------------------------------------------

//EicToyModel *EicToyModel::DefineVacuumChamber(const char *fname)
//{
//  
//return this; 
//} // EicToyModel::DefineVacuumChamber()

// ---------------------------------------------------------------------------------------

G4VPhysicalVolume *EicToyModel::ConstructG4World( void )
{
#ifdef _ETM2GEANT_
  auto air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  double cff = cm/etm::cm;
  // Define the "experimental hall"; units are TGeo [cm], as stored;
  auto expHall_box = new G4Box("World",  
			       cff*GetIrRegionRadius(),
			       cff*GetIrRegionRadius(),
			       cff*GetIrRegionLength()/2);
  auto expHall_log = new G4LogicalVolume(expHall_box, air, "World", 0, 0, 0);
  
  return new G4PVPlacement(0, G4ThreeVector(), expHall_log, "World", 0, false, 0);
#else
  return 0;
#endif
} // EicToyModel::ConstructG4World()

// ---------------------------------------------------------------------------------------

void EicToyModel::PlaceG4Volumes(G4LogicalVolume *world)
{
  for(auto stack: mStacks)
    for(auto det: stack->mDetectors)
      det->PlaceG4Volume(world);
} // EicToyModel::PlaceG4Volumes()

// ---------------------------------------------------------------------------------------

void EicToyModel::PlaceG4Volumes(G4VPhysicalVolume *world)
{
#ifdef _ETM2GEANT_
  PlaceG4Volumes(world->GetLogicalVolume());
#endif
} // EicToyModel::PlaceG4Volumes()

// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::acceptance(double eta0, double eta1, double eta2, double eta3, 
				     bool reset_stacks, bool redraw)
{
  // Order the values;
  std::set<double> buffer;
  buffer.insert(eta0); buffer.insert(eta1); buffer.insert(eta2); buffer.insert(eta3); 

  if (buffer.size() != 4) {
    printf("At least two of the 'eta' values given to EicToyModel::acceptance() are the same!\n");
    return this;
  } //if

  for(auto stack: mStacks) 
    for(unsigned bf=0; bf<2; bf++)
      // Perhaps the same eta will be "removed" more than once; ignore;
      mEtaLines.erase(stack->EtaBoundary(bf)->GetEta());

  for(auto eta: buffer) 
    // Request only the label to be printed out by DrawEtaLines(); the crack itself will 
    // be drawn by DrawEtaBoundaries(); FIXME: may want to unify these two functionalities, 
    // since they are anyway calling the same CalculateEtaEndPoint() internally;
    AddEtaLine(eta, false, true);

  if (reset_stacks) {
    mStacks.clear();

    // vtx(), bck(), mid(), fwd() to the moment;
    const char *labels[] = {"VTX", "BCK", "MID", "FWD"};
    for(unsigned st=0; st<4; st++)
      mStacks.push_back(new EtmDetectorStack(labels[st]));
  } //if
  
    // Sequence is: vtx(), bck(), mid(), fwd(); NB: it is essential that vtx() is earlier
    // than mid(), since its distance calculation is used to determine mid() offset;
  vtx()->SetEtaBoundaries(eta1, eta2);
  bck()->SetEtaBoundaries(eta0, eta1);
  mid()->SetEtaBoundaries(eta1, eta2);
  fwd()->SetEtaBoundaries(eta2, eta3);

  if (redraw && mCanvas) DrawMe(); 

  return this;
} // EicToyModel::acceptance()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void EicToyModel::CalculateDefaultEtaBoundaries(EtmDetectorStack *single)
{
  auto ip = GetIpLocation();
  TVector2 ip_down(mIpOffset, -1E-6); 
  double dx[2] = {0.0, 0.0};

  if (!single) {
    for(unsigned bf=0; bf<2; bf++) {
      auto boundary = vtx()->mEtaBoundaries[bf];
      // FIXME: do not care about vh/tb?; should be the same, or?;
      auto &vtxarr = boundary->mCrackNodes[0][0];
      
      dx[bf] = vtxarr.size() > 2 ? vtxarr[vtxarr.size()-2].X() : 0.0;
    } //for bf
  } //if

  auto ll = EtmLine2D(TVector2(dx[0], 0), TVector2(0,1));
  auto rr = EtmLine2D(TVector2(dx[1], 0), TVector2(0,1));

  for(auto stack: mStacks) {
    if (single && stack != single) continue;

    for(unsigned bf=0; bf<2; bf++) {
      bool account_crossing_angle = stack == fwd() && bf;
      auto boundary = stack->mEtaBoundaries[bf];

      for(unsigned vh=0; vh<2; vh++) {
	for(unsigned tb=0; tb<2; tb++) {
	  double tbsign = tb ? -1.0 : 1.0;
	  double dyv = tbsign*vtx()->FullLength();
	  double dym = tbsign*mid()->mStartingDistance;

	  auto lcv = EtmLine2D(ip + TVector2( 0,dyv), TVector2(1,0));
	  auto lcm = EtmLine2D(ip + TVector2( 0,dym), TVector2(1,0));

	  TVector2 ip_native = CalculateEtaStartPoint(boundary->GetEta(), vh, tb, 
						      account_crossing_angle);

	  auto &vtxarr = boundary->mCrackNodes[vh][tb];
	  vtxarr.clear();

	  if (stack == vtx() || !dyv || 
	      (stack == mid() &&  bf && !dx[1]) ||
	      (stack == bck() && !bf && !dx[0]) || 
	      (stack == fwd() &&  bf && !dx[1])) 
	    vtxarr.push_back(ip_native);
	  else {
	    auto L  = CalculateEtaEndPoint(boundary->GetEta(), vh, tb, 
					   account_crossing_angle).first; 
	    auto arrow = L - ip, n = arrow.Unit();
	    auto l0 = EtmLine2D(ip, n);

	    vtxarr.push_back(ip_down);

	    // These are mutually exclusive; no 'else if' for better readability;
	    if ((stack == bck() &&  bf) || stack == mid() || (stack == fwd() && !bf)) {
	      vtxarr.push_back(TVector2(ip.X(), dym));
	      vtxarr.push_back(TVector2(lcm.Cross(l0)));
	    } //if
	    if (stack == bck() && !bf) {
	      vtxarr.push_back(TVector2(ip.X(), dyv));
	      vtxarr.push_back(TVector2(lcv.Cross(ll)));
	      vtxarr.push_back(l0.Cross(ll));
	    }
	    if (stack == fwd() && bf) {
	      vtxarr.push_back(TVector2(ip.X(), dyv));
	      vtxarr.push_back(TVector2(lcv.Cross(rr)));
	      vtxarr.push_back(l0.Cross(rr));
	    }
	  } //if

	  // This is not clean, but should suffice for all practical purposes;
	  // NB: do not want to account for the crossing at eta~1.0, for sure 
	  // (integration volume creating will become overcomplicated);
	  vtxarr.push_back(CalculateEtaEndPoint(boundary->GetEta(), vh, tb, 
						account_crossing_angle).first); 
	} //for tb
      } //for vh
    } //for bf
  } //for stack
} // EicToyModel::CalculateDefaultEtaBoundaries()

// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::ip(double offset, bool redraw) 
{ 
  if (!mGeometryLocked && offset != mIpOffset) {
    mIpOffset = offset; 
    if (mVacuumChamber) mVacuumChamber->CheckGeometry(true);
    
    if (redraw && mCanvas) DrawMe(); 
  } //if

  return this; 
} // EicToyModel::ip()

// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::ir(double length, double radius, bool redraw) { 
  mIrRegionLength = length; mIrRegionRadius = radius; 
  
  home(redraw); 

  return this; 
} // EicToyModel::ir()

// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::RebuildEverything(bool redraw)
{ 
  if (mVacuumChamber) mVacuumChamber->CheckGeometry();
  
  if (redraw && mCanvas) DrawMe(); 

  return this; 
} // EicToyModel::RebuildEverything()

// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::SetCrossingAngle(double value, bool redraw) 
{
  // Same value -> nothing to do;
  if (value == mCrossingAngle) return this;

  // Change is only allowed if vacuum chamber design is consistent with it;
  if (!mVacuumChamber || mVacuumChamber->CrossingAngleResetPossible(value))
    mCrossingAngle = value; 

  return RebuildEverything(redraw);
} // EicToyModel::SetCrossingAngle()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::width(unsigned width) 
{ 
  mXdim = width; mNewCanvasRequired = true;
  home(); 
  
  return this; 
} // EicToyModel::width()

// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::mirror(bool what, bool redraw) 
{ 
  if (mMirrorImage != what) {
    mMirrorImage = what; 
    mNewCanvasRequired = true;
    home(redraw); 
  } //if

  return this; 
} // EicToyModel::mirror()

// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::legend(bool what, bool redraw) 
{ 
  mColorLegendEnabled = what; 
  home(redraw); 

  return this; 
} // EicToyModel::legend()

// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::AddEtaLine(double value, bool line, bool label, bool redraw) 
{ 
  mEtaLines[value] = std::make_pair(line, label); 
  
  if (redraw && mCanvas) DrawMe(); 

  return this; 
} // EicToyModel::AddEtaLine()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void EicToyModel::write(bool everything, bool lock)
{
  Export((GetName() + TString(".root")).Data(), everything, lock);
} // EicToyModel::write()

// ---------------------------------------------------------------------------------------

void EicToyModel::ExportVacuumChamber(const char *fname)
{
  TString str(fname ? TString(fname) : GetName() + TString(".vc.gdml"));

  if (str.EndsWith(".gdml")) {
    if (mVacuumChamber)
      mVacuumChamber->Export(str.Data());
    else
      printf("EicToyModel::ExportVacuumChamber() -> vacuum chamber is not defined!\n");
  }
  else
    printf("EicToyModel::ExportVacuumChamber(\"%s\") -> .gdml file extension expected!\n", 
	   fname); 
} // EicToyModel::ExportVacuumChamber()

// ---------------------------------------------------------------------------------------

int EicToyModel::ExportCADmodel(const char *fname)
{
  if (GetAzimuthalSegmentation()) {
    printf("\n\nCan not (yet) export azimuthally-segmented CAD model!\n\n");
    return -1;
  } //if

#ifdef _OPENCASCADE_
  // Create XCAF document;
  Handle(TDocStd_Document) outDoc;
  Handle(XCAFApp_Application) outApp = XCAFApp_Application::GetApplication(); 
  outApp->NewDocument("ETM", outDoc); 
  
  Handle (XCAFDoc_ShapeTool) outAssembly = XCAFDoc_DocumentTool::ShapeTool (outDoc->Main()); 
  
  Handle(XCAFDoc_ColorTool) outColors = XCAFDoc_DocumentTool::ColorTool(outDoc->Main()); 
  
  for(auto stack: mStacks)
    for(auto det: stack->mDetectors) {
      // Can be a GAP or a MARKER detector;
      if (!det->Polygons().size()) continue;
      
      auto polygon = det->Polygons()[0];
      unsigned dim = polygon.size();
      double cff = 1.0;//cm/etm::cm;
      
      //auto eic = EicToyModel::Instance();
      
      // Treat Z-offsets differently for central/vertex and endcap detectors;
      //TVector2 ip = eic->GetIpLocation();
      //double z0 = (mStack == eic->bck() || mStack == eic->fwd()) ? 
      //(ip + mActualDistance*mStack->AlignmentAxis()).X() : ip.X();
      double z0 = 0.0;
      
      // Create OpenCascade polygon;
      auto poly = BRepBuilderAPI_MakePolygon();
      for(unsigned ivtx=0; ivtx<dim; ivtx++) {
	auto &pt = polygon[ivtx];
	
	poly.Add(gp_Pnt(pt.Y()*cff, 0.0, (pt.X()-z0)*cff));
	
	//z[ivtx] = (pt.X() - z0)*cff; 
	//r[ivtx] = pt.Y()*cff; 
	
	//printf("%7.2f %7.2f\n", pt.X(), pt.Y());
      } //for vtx
      poly.Close();
      
      // Build a 2D face out of it; then create a revolution body;
      auto face = BRepBuilderAPI_MakeFace(poly);
      gp_Ax1 axis(gp_Pnt(0,0,0), gp_Dir(0,0,1)); 
      auto solid = BRepPrimAPI_MakeRevol(face, axis); 
      
      // Assign ROOT TGeo color;
      {
	auto rcolor = gROOT->GetColor(det->GetFillColor());
	
	Quantity_Color ccolor(rcolor->GetRed(), rcolor->GetGreen(), rcolor->GetBlue(), Quantity_TOC_RGB);
	TDF_Label aLabel = outAssembly->AddShape(solid);
	outColors->SetColor(aLabel, ccolor, XCAFDoc_ColorSurf);
      }
    } //for stack .. det
  
      // Write the file out;
  {
    STEPCAFControl_Writer cWriter;
    
    cWriter.Transfer(outDoc, STEPControl_ManifoldSolidBrep);
    //cWriter.Transfer(outDoc, STEPControl_AsIs);
    cWriter.Write(fname);
  }

  return 0;
#else
  return -1;
#endif
} // EicToyModel::ExportCADmodel()

// ---------------------------------------------------------------------------------------
//
// FIXME: unify with EtmDetector::Export(); evil OCC handles;
//

void EicToyModel::Export(const char *fname, bool everything, bool lock)
{
  if (fname) {
    TString str(fname);

    if (str.EndsWith(".root")) {
      TFile fout(str.Data(), "RECREATE");

      // Permanently lock the geometry if requested;
      if (lock) mGeometryLocked = true;

      if (everything) {
	if (mVacuumChamber) {
	  // Save vacuum chamber TGeo tree in the same file;
	  mVacuumChamber->GetWorld()->CloseGeometry();
	  mVacuumChamber->GetWorld()->CheckOverlaps(0.0001);
	  mVacuumChamber->GetWorld()->Write();
	  
	  // And also store GDML dump as a separate TObjString; this is kind of redundant
	  // since one can always restore this GDML dump using TGeoManager dump; however 
	  // technically there can be a situation where gGeoManager is initialized already, 
	  // and it may preferrable not to disturb it; anyway, the whole idea behind this 
	  // dump is to provide yeat another option to import the vacuum system geometry 
	  // without loading ETM library (the dump is store as a separate object);
	  mVacuumChamber->StoreGDMLdump();
	} //if

#ifdef _OPENCASCADE_
	{
	  // FIXME: unify with EtmVacuumChamber::StoreGDMLdump();
	  const char *qfname = "/tmp/tmp.stp";

	  if (!ExportCADmodel(qfname)) {
	    std::ifstream fin(qfname);
	    
	    TString str;
	    str.ReadFile(fin);
	    
	    TObjString ostr; ostr.SetString(str);
	    // FIXME: hardcoded (Central Detector);
	    ostr.Write("CD.STEP");
	    
	    // Remove the temporary file;
	    unlink(qfname);
	  } //if
	}
#endif
      } //if

      // Save EicToyModel class instance itself (ROOT serializer);
      TObject::Write("EicToyModel");

      fout.Close();
    } else if (str.EndsWith(".stp")) {
#ifdef _OPENCASCADE_
      ExportCADmodel(fname);
#else
      printf("EicToyModel::Export(\"%s\") -> OpenCascade support is not compiled in!\n", fname);
#endif
    } else {
      printf("EicToyModel::Export(\"%s\") -> unknown file name extension!\n", fname);
    } //if
  } else
    printf("EicToyModel::Export() -> null pointer given as file name!\n");
} // EicToyModel::Export()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

EtmDetectorStack *EicToyModel::vtx( void ) 
{ 
  return (mStacks.size() >= 1 ? mStacks[0] : 0);
} // EicToyModel::vtx()

// ---------------------------------------------------------------------------------------

EtmDetectorStack *EicToyModel::bck( void ) 
{ 
  return (mStacks.size() >= 2 ? mStacks[1] : 0);
} // EicToyModel::bck()

// ---------------------------------------------------------------------------------------

EtmDetectorStack *EicToyModel::mid( void ) 
{ 
  return (mStacks.size() >= 3 ? mStacks[2] : 0);
} // EicToyModel::mid()

// ---------------------------------------------------------------------------------------

EtmDetectorStack *EicToyModel::fwd( void ) 
{ 
  return (mStacks.size() >= 4 ? mStacks[3] : 0);
} // EicToyModel::fwd()

// ---------------------------------------------------------------------------------------

EtmDetectorStack *EicToyModel::GetStack(double eta) 
{ 
  for(auto stack: mStacks)
    if (stack != vtx() && stack->etamin() <= eta && eta < stack->etamax()) return stack;

  // Yet consider a special case;
  if (eta == fwd()->EtaBoundary(1)->GetEta()) return fwd();

  return 0;
} // EicToyModel::GetStack()

// ---------------------------------------------------------------------------------------

EicToyModel *EicToyModel::DrawFlatFieldLines(double eta) {
  mFlatFieldLinesEta.insert(eta);

  return this;
} // EicToyModel::DrawFlatFieldLines()

// ---------------------------------------------------------------------------------------

TVector2 EicToyModel::cnv(const TVector2 &world) const
{
  return TVector2((world.X() - mX0)/mXsize, (world.Y() - mY0)/mYsize);
} // EicToyModel::cnv()

// ---------------------------------------------------------------------------------------

void EicToyModel::home(bool redraw)
{
  // NB: this does not affect the canvas size (will be preserved);
  mXsize = (mOneSideMode == EicToyModel::kOff) ? (mIrRegionLength + 2*_IR_REGION_SMARGIN_) : 
    (mIrRegionLength/2 +  3*_IR_REGION_SMARGIN_);
  mYsize = (mMirrorImage ? 2 : 1)*mIrRegionRadius + 2*_IR_REGION_BMARGIN_ + 
    (mColorLegendEnabled ? _COLOR_LEGEND_HEIGHT_ : 0.0);
  switch (mOneSideMode) {
  case EicToyModel::kOff:
    mX0 = -mXsize/2;
    break;
  case EicToyModel::kLeft:
    mX0 = -mXsize + 2*_IR_REGION_SMARGIN_;
    break;
  case EicToyModel::kRight:
    mX0 = -2*_IR_REGION_SMARGIN_;
    break;
  } //switch
  mY0    = (mMirrorImage ? -1 : 0)*mIrRegionRadius - _IR_REGION_BMARGIN_;

  // Well, this is not 100% correct, but a new window will almost certainly be needed;
  mNewCanvasRequired = true;

  mZoomedView = false;

  if (redraw && mCanvas) DrawMe();
} // EicToyModel::home()

// ---------------------------------------------------------------------------------------

void EicToyModel::zoom(double blX, double blY, double trX, double trY, bool redraw)
{
  mXsize = fabs(trX - blX); 
  mYsize = fabs(trY - blY); 
  mX0    = blX < trX ? blX : trX;
  mY0    = blY < trY ? blY : trY;

  if (ConditionChecker(mXsize && mYsize, "EicToyModel::zoom() call: zero width (height)")) {
    mZoomedView = true;

    // Well, this is not 100% correct, but a new window will almost certainly be needed;
    mNewCanvasRequired = true;

    if (redraw && mCanvas) DrawMe();
  } //if
} // EicToyModel::zoom()

// ---------------------------------------------------------------------------------------

void EicToyModel::AddBoundaryModificationRequest(EtmDetector *hostdet, EtmDetector *refdet, 
						 double toffset, etm::Stretch how)
{
  mBoundaryModificationRequests.
    push_back(new EtmBoundaryModificationRequest(hostdet, refdet, toffset, how));
} // EicToyModel::AddBoundaryModificationRequest()

// ---------------------------------------------------------------------------------------

std::vector<TVector2> EicToyModel::Band(const std::vector<TVector2> &chain, 
					const EtmLine2D &from, const EtmLine2D &to)
{
  unsigned dim = chain.size();
  std::vector<TVector2> ret; 
  
  // FIXME: need to place from->to order check;

  // 'From' line: go up, away from the IP, and start filling the buffer as soon as 
  // the distance goes positive; from that point on check the 'to' condition;
  for(unsigned ivtx=0; ivtx<dim; ivtx++) {
    auto const &vcurr0 = chain[ivtx];
    double d0 = from.Distance(vcurr0);

    if (d0 >= 0.0) {
      // First node; ivtx!=0 since IP must certainly be bad (?);
      assert(ivtx);
      auto const &vprev0 = chain[ivtx-1];
      
      EtmLine2D ll(vcurr0, (vcurr0-vprev0).Unit());
      if (from.IsParallel(ll)) continue;
      //assert(!from.IsParallel(ll));
      ret.push_back(from.Cross(ll));

      for(unsigned jvtx=ivtx; jvtx<dim; jvtx++) {
	auto const &vcurr1 = chain[jvtx];
	double d1 = to.Distance(vcurr1);
      
	if (d1 <= 0.0) {
	  ret.push_back(vcurr1);
	  continue;
	} else {
	  // For sure ret.size() != 0 here;
	  //assert(jvtx);
	  auto const &vprev1 = chain[jvtx-1];
	  
	  EtmLine2D ll(vcurr1, (vcurr1-vprev1).Unit());
	  ret.push_back(to.Cross(ll));
	  return ret;
	} //if
      } //for jvtx
    } //if
  } //for ivtx

  // Make the compiler happy;
  return ret;
} // EicToyModel::Band()

// ---------------------------------------------------------------------------------------

std::pair<std::vector<TVector2>, std::vector<TVector2> > 
EicToyModel::GetIntersection(const std::vector<TVector2> &chain,
			     const EtmLine2D &line)
{
  unsigned dim = chain.size();
  std::pair<std::vector<TVector2>, std::vector<TVector2> > ret;
  std::vector<TVector2> &before = ret.first, &after = ret.second;

  for(unsigned ivtx=0; ivtx<dim; ivtx++) {
    auto const &vcurr = chain[ivtx];

    //printf("%7.2f %7.2f -> %7.2f\n", vcurr.X(), vcurr.Y(), line.Distance(vcurr));
    if (line.Distance(vcurr) < 0.0)
    //if (line.Distance(vcurr) <= 0.0)
      before.push_back(vcurr);
    else {
      auto const &vprev = chain[ivtx-1];
      EtmLine2D lcross(vcurr, (vcurr-vprev).Unit());
      //assert(!line.IsParallel(lcross));
      if (!line.IsParallel(lcross)) {
	// Yes, account the crossing point in both sub-chains;
	before.push_back(line.Cross(lcross));
	after .push_back(line.Cross(lcross));
	after .push_back(vcurr);
      
	for(unsigned jvtx=ivtx+1; jvtx<dim; jvtx++) 
	  after.push_back(chain[jvtx]);
	
	return ret;
      } //if
    } //if
  } //for ivtx

  // Make compiler happy;
  return ret;
} // EicToyModel::GetIntersection()

// ---------------------------------------------------------------------------------------

void EicToyModel::AppendChain(std::vector<TVector2> &chain, const std::vector<TVector2> &add)
{
  // FIXME: use insert();
  //for(unsigned ivtx=0; ivtx<add.size(); ivtx++)
  //chain.push_back(add[ivtx]);

  chain.insert(chain.end(), add.begin(), add.end());
} // EicToyModel::AppendChain()

// ---------------------------------------------------------------------------------------

std::vector<TVector2> EicToyModel::RebuildCrack(const std::vector<TVector2> &chain, 
						const EtmLine2D &am, const EtmLine2D &ap,
						const EtmLine2D &cut, etm::Stretch how)
{
  std::vector<TVector2> ret;
  // Prefer clarity rather than efficiency;
  std::pair<std::vector<TVector2>, std::vector<TVector2> > cm = GetIntersection(chain, am);
  std::pair<std::vector<TVector2>, std::vector<TVector2> > cp = GetIntersection(chain, ap);
  std::pair<std::vector<TVector2>, std::vector<TVector2> > cc = GetIntersection(chain, cut);
  // Relative position of this point with respect to the [am..ap] band defines the 
  // output chain; then there are three options how to modify the output, on top of this;
  //printf("%d %d\n", cc.first.size(), cc.second.size());
  const TVector2 &C = cc.second[0];
  double dcm = am.Distance(C), dcp = ap.Distance(C); 

  // Few more useful 2D points;
  const TVector2 &L = chain[chain.size()-1];
  const TVector2 /*&A = cp.second[0], &B = cm.second[0],*/ &P = cut.Cross(am), &Q = cut.Cross(ap);
  // FIXME: this is not good;
  bool vcut = fabs(cut.V().X()) < fabs(cut.V().Y());
  TVector2 D = TVector2(vcut ? Q.X() : L.X(), vcut ? L.Y() : Q.Y());

  if (dcm <= 0.0 && dcp < 0.0) {
    switch (how) {
    case etm::kDent:
      return chain;
    case etm::kRecess:
      AppendChain(ret, cc.first);
      //ret.push_back(B); 
      //ret.push_back(P); 
      ret.push_back(Q); 
      AppendChain(ret, cp.second);
      break;
    case etm::kWall:
      AppendChain(ret, cc.first);
      //ret.push_back(B); 
      //ret.push_back(P); 
      ret.push_back(D); 
      ret.push_back(L); 
      break;
    } //switch
  } else if (dcm > 0.0 && dcp < 0.0) {
    switch (how) {
    case etm::kDent:
      AppendChain(ret, cm.first);
      ret.push_back(P); 
      AppendChain(ret, cc.second);
      break;
    case etm::kRecess:
      AppendChain(ret, cm.first);
      ret.push_back(P); 
      ret.push_back(Q); 
      AppendChain(ret, cp.second);
      break;
    case etm::kWall:
      AppendChain(ret, cm.first);
      ret.push_back(P); 
      ret.push_back(D); 
      ret.push_back(L); 
      break;
    } //switch
  } else {
    switch (how) {
    case etm::kDent: 
      //AppendChain(ret, cm.first);
      //ret.push_back(P); 
      //ret.push_back(Q);
      //ret.push_back(A); 
      //AppendChain(ret, cp.second);
      //break;
      ;
    case etm::kRecess:
      AppendChain(ret, cm.first);
      ret.push_back(P); 
      AppendChain(ret, cc.second);
      break;
    case etm::kWall:
      AppendChain(ret, cm.first);
      ret.push_back(P); 
      ret.push_back(D); 
      ret.push_back(L); 
      break;
    } //switch
  } //if

  return ret;
} // EicToyModel::RebuildCrack()

// ---------------------------------------------------------------------------------------

void EicToyModel::ExecuteBoundaryModificationRequests(EtmDetectorStack *single)
{
  CalculateDefaultEtaBoundaries(single);

  // Loop through all the request *in*the*order*they*were*submitted* in a .C script;
  for(auto request: mBoundaryModificationRequests) {
    auto hstack = request->mHostDetector->mStack;
    if (single && hstack != single) continue;

    auto rstack = request->mReferenceDetector->mStack;
    auto host = request->mHostDetector, ref = request->mReferenceDetector;

    // NB: 'ref' pointer may be to a 'dummy' detector (to be ignored);
    if (host) {
      bool order = rstack->EtaBoundary(0)->GetEta() > hstack->EtaBoundary(0)->GetEta();
      EtmEtaBoundary *boundaries[2] = {
	hstack->EtaBoundary( order), 
	// NB: in case of the vtx() stack CalculateDefaultEtaBoundaries() did the job
	// to correct either bck() or fwd() beamline boundaries; RebuildCrack() would 
	// not work anyway because crack lines are not monotonous;
	hstack == vtx() ? 0 : rstack->EtaBoundary(!order)
      };

      auto ip = GetIpLocation();		
            
      for(unsigned vh=0; vh<2; vh++) {
	for(unsigned tb=0; tb<2; tb++) {
	  double dthost = host->GetActualDistance();
	  double dtref = (ref && !ref->IsDummy()? ref->GetActualDistance() - ref->length()/2 : 0.0) + 
	    request->mTransverseOffset;

	  TVector2 aaxis(hstack->mAlignmentAxis.X(), (tb ? -1.0 : 1.0)*hstack->mAlignmentAxis.Y());
	  TVector2 caxis(rstack->mAlignmentAxis.X(), (tb ? -1.0 : 1.0)*rstack->mAlignmentAxis.Y());

	  // So it seems like once the alignment axes are oriented away from the IP, it is the 
	  // azimuthal symmetry used for signed point-to-line distance what matters -> rotate by 
	  // -pi/2 in all cases; 
	  auto  am = EtmLine2D(ip + (dthost - host->length()/2    /*- SafetyClearance()/2*/)*aaxis, 
			       aaxis.Rotate(-M_PI/2));
	  auto  ap = EtmLine2D(ip + (dthost + host->length()/2    /*+ SafetyClearance()/2*/)*aaxis, 
			       aaxis.Rotate(-M_PI/2));
	  auto cut = EtmLine2D(ip + (dtref /*- hcal->length()/2*/ /*- SafetyClearance()/2*/)*caxis, 
	  		       caxis.Rotate(-M_PI/2));
	  // Range check; unify with EtmDetector::Build();
	  {
	    //double dmax = fabs(to.X().X());
	    bool central = host->mStack == vtx() || host->mStack == mid();
	    
	    // Yes, I know they are exclusive;
	    if ( central && fabs(cut.X().X()) > GetIrRegionLength()/2) return; 
	    if (!central && fabs(cut.X().Y()) > GetIrRegionRadius())   return; 
	  }
	  
	  for(unsigned bf=0; bf<2; bf++) {
	    auto boundary = boundaries[bf]; 
	    if (!boundary) continue;

	    auto &vtxarr = boundary->mCrackNodes[vh][tb];

	    vtxarr = RebuildCrack(vtxarr, am, ap, cut, request->mStretch);
	  } //for bf
	} //for tb
      } //for vh
    } //if
  } //for request
} // EicToyModel::ExecuteBoundaryModificationRequest()

// ---------------------------------------------------------------------------------------

void EicToyModel::ApplyStandardTrimming( void )
{
  // FIXME: this is a bit cubmersome: modify all edges except for those which stay 
  // at _DEFAULT_TRIM_VALUE_;
  for(auto det: bck()->mDetectors)
    det->trim(0.0, _DEFAULT_TRIM_VALUE_, true);

  for(auto det: mid()->mDetectors)
    det->trim(_DEFAULT_TRIM_VALUE_, _DEFAULT_TRIM_VALUE_, true);

  for(auto det: fwd()->mDetectors)
    det->trim(_DEFAULT_TRIM_VALUE_, 0.0, true);
} // EicToyModel::ApplyStandardTrimming()

// ---------------------------------------------------------------------------------------

void EicToyModel::AccountServiceMaterials( void )
{
#if _TODAY_
  for(auto boundary: mEtaBoundaries) 
    for(unsigned vh=0; vh<2; vh++) {
      auto &wdarr = boundary->mCrackWidths[vh];
      
      wdarr.clear();
      
      // Per default these will just be safety clearance values;
      for(unsigned iq=0; iq<boundary->mCrackNodes[vh][0].size()-1; iq++)
	wdarr.push_back(SafetyClearance());
    } //for boundary..vh
#endif
} // EicToyModel::AccountServiceMaterials()

// ---------------------------------------------------------------------------------------

void EicToyModel::DrawMe(EicToyModel::View view, bool draw) 
{
  if (view != kUndefined && view != kCurrent) mCurrentView = view;

  // These need to be re-calculated only for the first entry;
  if (!mCanvas) home(false);

  // This is now in pixels;
  mYdim = mXdim * mYsize / mXsize;

  if (draw) {
    gStyle->SetCanvasColor(0);

    if (mNewCanvasRequired) {
      // If canvas was used already, delete it;
      if (mCanvas) delete mCanvas;

      mCanvas = new TCanvas(GetName(), GetName(), 0, 0, mXdim, mYdim);

      mNewCanvasRequired = false;
    } else {
      // If canvas was used already, clean it;
      if (mCanvas) mCanvas->Clear();
    } //if
  } //if

  // FIXME: is the recalculation needed here?; well, something was crashing ...;
  if (!mGeometryLocked) {
    // Calculate locations of all the detectors;
    for(auto stack: mStacks)
      stack->CalculateActualDistances();

    // FIXME: well, this is a bit too muc of a complication: first process all 
    // the requests, which allow one to define the "size" of the vertex stack; 
    // then make a second pass, where this information is accounted to build 
    // beamline boundaries of bck() and fwd() stacks; NB: can not create per-stack
    // stretch() call vectors because these requests do not commute for different
    // stacks (so the sequence matters);
    ExecuteBoundaryModificationRequests(vtx());
    ExecuteBoundaryModificationRequests();

    //++PurgeBoundaryVertexArrays();
    AccountServiceMaterials();

    // And now when all the stack boundaries are calculated, eventually 
    // build the detectors;
    for(auto stack: mStacks) 
      stack->Build();
  } //if

  unsigned vh = GetCurrentView() == EicToyModel::kVertical ? 0 : 1;

  // Loop through all stacks and build the detectors; populate the mirror image
  // if needed; 
  for (unsigned tb=0; tb<2; tb++) {
    if (tb && !mMirrorImage) continue;

    for(auto stack: mStacks) 
      // Draw them; yes, prefer to decouple building and drawing;
      for(auto det: stack->mDetectors) 
	if (!mEnabledColorTags.size() || 
	    mEnabledColorTags.find(*det->GetLabel()) != mEnabledColorTags.end())
	  if (det->Polygons().size()) 
	    det->DrawPolygon(det->Polygons()[vh*2+tb]);
  } //for tb

  if (draw) {
    // Start with the area, which describes the beam pipe;
    if (mVacuumChamber) mVacuumChamber->DrawMe();
    
    // Draw all booked acceptance limit lines; this looks a bit artificial, but I want to 
    // decouple the actual acceptance boundary lines from the additional "drawn-only" ones;
    DrawEtaLines();
    DrawEtaBoundaries();
    
    // Draw the "beam line";
    DrawBeamLine();
    
    // Draw meter and half a meter ticks;
    DrawAxisTicks();
    
    // Draw regions where the field is supposed to be aligned with the solenoid axis;
    DrawFlatFieldLines();
    
    DrawMarkers();
    
    // Draw color map;
    if (mColorLegendEnabled && !mZoomedView) 
      DrawColorLegend(mOneSideMode == EicToyModel::kOff ? 1 :2);
  } //if
} // EicToyModel::DrawMe()

// ---------------------------------------------------------------------------------------

void EicToyModel::WriteText(const TVector2 &where, const TString &what, int color) const
{
  TVector2 coord = cnv(where);

  // FIXME: here and in many other places: a memory leak;
  auto text = new TText(coord.X(), coord.Y(), what);

  text->SetTextAlign(22);
  text->SetTextColor(color);
  text->SetTextFont(43);
  text->SetTextSize(20*mXdim*(mOneSideMode == EicToyModel::kOff ? 1 : 2)/_CANVAS_WIDTH_DEFAULT_);
  text->Draw();
} // EicToyModel::WriteText()

// ---------------------------------------------------------------------------------------
#if 1
void EicToyModel::DrawPolygon(unsigned dim, const double xx[], const double yy[], int color, 
			  bool line) const
{
  double xxc[dim+1], yyc[dim+1];

  for(unsigned iq=0; iq<dim; iq++) {
    auto xyc = cnv(TVector2(xx[iq], yy[iq]));
    xxc[iq] = xyc.X(); yyc[iq] = xyc.Y();
  } //for iq
  xxc[dim] = xxc[0]; yyc[dim] = yyc[0];

  // FIXME: memory leak;
  TPolyLine *poly = new TPolyLine(dim+1, xxc, yyc);

  poly->SetLineColor(kBlack);
  poly->SetFillColor(color);
  poly->SetLineWidth(1);
  poly->SetLineStyle(etm::solid);
  poly->Draw("F");
  if (line) poly->Draw();
} // EicToyModel::DrawPolygon()

// ---------------------------------------------------------------------------------------

void EicToyModel::DrawRectangle(const TVector2 &bl, const TVector2 &tr, int color, bool line) const
{
  double xx[4] = {bl.X(), tr.X(), tr.X(), bl.X()};
  double yy[4] = {bl.Y(), bl.Y(), tr.Y(), tr.Y()};

  DrawPolygon(4, xx, yy, color, line);
} // DrawRectangle()
#endif
// ---------------------------------------------------------------------------------------

void EicToyModel::DrawSingleLine(const TVector2 &from, const TVector2 &to, int color, int width,
			     etm::LineStyle style) const
{
  TVector2 p1 = cnv(from), p2 = cnv(to);
  double xx[2] = {p1.X(), p2.X()}, yy[2] = {p1.Y(), p2.Y()};
  
  // FIXME: memory leak;
  TPolyLine *poly = new TPolyLine(2, xx, yy);
  
  poly->SetLineColor(color);
  poly->SetLineWidth(width);
  poly->SetLineStyle(style);
  poly->Draw();
} // EicToyModel::DrawSingleLine()
 
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

TVector2 EicToyModel::CalculateEtaStartPoint(double eta, unsigned vh, unsigned tb, 
					     bool account_crossing_angle)
{
  double theta = Eta2Theta(eta), tbsign = tb ? -1.0 : 1.0;

  if (eta > 0.0 && vh && account_crossing_angle) theta += tbsign*mCrossingAngle;

  TVector2 dxy = 1E-6*TVector2(1,0).Rotate(theta);
  return GetIpLocation() - TVector2(dxy.X(), tbsign*dxy.Y()); 
} // EicToyModel::CalculateEtaStartPoint()

// ---------------------------------------------------------------------------------------

std::pair<TVector2, bool> EicToyModel::CalculateEtaEndPoint(double eta, unsigned vh, unsigned tb, 
							    bool account_crossing_angle)
{
  double theta = Eta2Theta(eta), tbsign = tb ? -1.0 : 1.0;

  if (eta > 0.0 && vh && account_crossing_angle) theta += tbsign*mCrossingAngle;

  {
    double dx = mIrRegionLength/2. + 1E-6;
    double x0 = (theta < TMath::Pi()/2 ? 1.0 : -1.0)*dx;
    double y0 = (x0 - mIpOffset)*tan(theta);

    if (fabs(y0) < mIrRegionRadius)
      return std::make_pair(TVector2(x0, tbsign*y0), true);
    else 
      return std::make_pair(TVector2(mIpOffset + mIrRegionRadius*tan(TMath::Pi()/2-theta), 
				     tbsign*mIrRegionRadius), false);
  }
} // EicToyModel::CalculateEtaEndPoint()

// ---------------------------------------------------------------------------------------

void EicToyModel::DrawSingleEtaLine(double eta, const TVector2 &to, 
			      std::pair<bool, bool> what, bool side) const
{
  // Line is drawn anyway;
  if (what.first) DrawSingleLine(GetIpLocation(), to, kBlack, 1, etm::dashdotted);

  // Label is optional;
  if (what.second) {
    TString label; label.Form("%4.2f", eta);
    
    // FIXME: offsets hardcoded;
    double sign = to.Y() > 0.0 ? 1.0 : -1.0;
    WriteText(to + TVector2(!side ? 0.0 : (eta < 0.0 ? -22.0 : 22.0), 
    			    !side ? sign*7.0*etm::cm : 0.0), label);
  } //if
} // EicToyModel::DrawSingleEtaLine()

// ---------------------------------------------------------------------------------------

void EicToyModel::DrawEtaLines( void )
{
  // Draw all booked eta lines;
  for(unsigned tb=0; tb<2; tb++) {
    if (tb && !mMirrorImage) continue;

    for(auto eta: mEtaLines) {
      auto pt = CalculateEtaEndPoint(eta.first, 
				     GetCurrentView() == EicToyModel::kHorizontal ? 1 : 0, tb, true);

      DrawSingleEtaLine(eta.first, pt.first, eta.second, pt.second);
    } //for eta
  } //for tb
} // EicToyModel::DrawEtaLines()

//bool account_crossing_angle = stack == fwd() && bf;
//  if (eta > 0.0 && vh && account_crossing_angle) theta += tbsign*mCrossingAngle;

// ---------------------------------------------------------------------------------------

void EicToyModel::DrawEtaBoundaries( void )
{
  unsigned vh = GetCurrentView() == EicToyModel::kHorizontal ? 1 : 0;

  for(unsigned tb=0; tb<2; tb++) {
    if (tb && !mMirrorImage) continue;

    for(auto stack: mStacks) {
      if (stack == vtx()) continue;

      for(unsigned im=0; im<2; im++) {
	auto boundary = stack->EtaBoundary(im);
	const auto &vtxarr = boundary->mCrackNodes[vh][tb];
	
	for(unsigned ivtx=1; ivtx<vtxarr.size(); ivtx++)
	  DrawSingleLine(vtxarr[ivtx-1], vtxarr[ivtx], kBlack, 1, etm::dashdotted);
      } //for im
    } //for stack
  } //for tb
} // EicToyModel::DrawEtaBoundaries()

// ---------------------------------------------------------------------------------------

void EicToyModel::DrawBeamLine( void )
{
  // Beam lines themselves;
  {
    TVector2 left(-mIrRegionLength/2., 0.0);
    TVector2 right(mIrRegionLength/2., GetCurrentView() == EicToyModel::kHorizontal ? 
		   tan(mCrossingAngle)*mIrRegionLength/2. : 0.0);
    // 25mrad solid line looks ugly, yet with the ticks it seems to be the best choice;
    DrawSingleLine(left,  GetIpLocation());//, kBlack, 1, etm::dashed);
    DrawSingleLine(GetIpLocation(), right);//, kBlack, 1, etm::dashed);
  }     
  for(unsigned tb=0; tb<2; tb++) {
    if (tb && !mMirrorImage) continue;

    // FIXME: hardcoded;
    double y0 = (tb ? -1.0 : 1.0)*_HALL_RADIAL_BOUNDARY_;

    DrawSingleLine(TVector2(-mIrRegionLength/2, y0), TVector2(mIrRegionLength/2, y0), 
		   kBlack, 1, etm::dashed);   
  } //for tb
} // EicToyModel::DrawBeamLine()

// ---------------------------------------------------------------------------------------

void EicToyModel::DrawMarkers( void )
{
  EtmDetectorStack *stacks[2] = {fwd(), bck()};

  for(unsigned fb=0; fb<2; fb++) {
    auto stack = stacks[fb];
    auto marker = stack->get(_MARKER_);
    
    if (!marker->IsDummy()) {
      TVector2 spot = GetIpLocation() + marker->mActualDistance*stack->AlignmentAxis(), dy(0.0, 5 * etm::cm);
      TVector2 from = cnv(spot - TVector2(0.0, 10 * etm::cm)), to = cnv(spot);

      // FIXME: memory leak;
      auto arrow = new TArrow(from.X(), from.Y(), to.X(), to.Y(), 0.005, "|>");
      arrow->SetLineWidth(1);
      arrow->SetLineColor(kRed);
      arrow->Draw();
    } //if
  } //for fb

  {
    double alength = 30.0 * etm::cm;

    for(unsigned lr=0; lr<2; lr++) {
      // These lines are too short, so a 25mrad angle would look ugly -> 0.0; 
      //double dr = (!lr && GetCurrentView() == EicToyModel::kHorizontal) ? 
      //-alength*tan(mVacuumChamber->mCrossingAngle) : 0.0;
      TVector2 from = cnv(TVector2(mIpOffset + (lr ? 1.0 : -1.0)*alength, /*dr*/0.0));
      TVector2 to = cnv(GetIpLocation());
      
      auto arrow = new TArrow(from.X(), from.Y(), to.X(), to.Y(), 0.007, "|>");
      arrow->SetLineWidth(2);
      arrow->SetLineColor(lr ? kBlue : kRed);
      arrow->Draw();
    } //for lr

    WriteText(TVector2(mIpOffset - 20.0, -10.0), "p", kRed);
    WriteText(TVector2(mIpOffset + 20.0, -10.0), "e", kBlue);
  }
} // EicToyModel::DrawMarkers()

// ---------------------------------------------------------------------------------------

void EicToyModel::DrawFlatFieldLines( void )
{
  for(auto eta: mFlatFieldLinesEta) {
    auto stack = GetStack(eta); 
    if (stack && (stack == fwd() || stack == bck())) {
      // First try to get MARKER object location;
      auto marker = stack->get(_MARKER_);
      
      if (!marker->IsDummy()) {
	double sign = (eta < 0.0 ? -1 : 1);
	double zmarker = marker ? sign*marker->GetActualDistance() + GetIpLocation().X() : 0.0;
	
	// FIXME: step hardcoded;
	double zmax = zmarker, rstep = 5 * etm::cm;
	double theta = Eta2Theta(eta), dz = zmax - GetIpLocation().X();
	double rmax = fabs(dz*tan(theta));
	
	for(unsigned tb=0; tb<2; tb++) {
	  if (tb && !mMirrorImage) continue;

	  for(unsigned iq=0; (iq+0)*rstep < rmax; iq++) { 
	    TVector2 from, to;
	    double y0 = (tb ? -1.0 : 1.0)*iq*rstep;
	    
	    from = TVector2(GetIpLocation().X() + iq*rstep/tan(theta), y0), to = TVector2(zmax, y0);
	    DrawSingleLine(from, to, kRed, 1, etm::dotted);
	  } //for iq

	  // Draw the surrounding triangle;
	  TVector2 apex(zmax, (tb ? -1.0 : 1.0)*rmax);
	  DrawSingleLine(apex, GetIpLocation(),     kRed, 1, etm::dotted);
	  DrawSingleLine(apex, TVector2(zmax, 0.0), kRed, 1, etm::dotted);
	} //for tb
      } //if
    } //if
  } //for eta
} // EicToyModel::DrawFlatFieldLines()

// ---------------------------------------------------------------------------------------

void EicToyModel::DrawAxisTicks( void )
{
  // Should just be large enough and odd;
  unsigned dim = 99;
  double step = 50.0 * etm::cm;
  for(unsigned iq=0; iq<dim; iq++) {
    double zOffset = step*(iq - (dim-1)/2.);
    
    if (abs(zOffset) > mIrRegionLength/2) continue;
    
    // So lift the ticks in case of a horizontal view, but does not lift the meter labels;
    double lift = (mMirrorImage ? ((iq%2) ? 5.0 : 2.5) * etm::cm : 0.0);
    double dlift = (zOffset > mIpOffset && GetCurrentView() == EicToyModel::kHorizontal) ?
      (zOffset - mIpOffset)*tan(mCrossingAngle) : 0.0;
    TVector2 from(zOffset, lift + dlift), to(zOffset, ((iq%2) ? -10.0 : -5.0) * etm::cm + lift + dlift);
    // '3': mark the nominal (0,0) point in some way;
    DrawSingleLine(from, to, kBlack, zOffset ? 1 : 3);
    
    // 1m labels; do not want to mess up when mirror image is present;
    if (!mMirrorImage && iq%2) {   
      TString label; 
      if (zOffset)
	label.Form("%c%5.1f", zOffset > 0.0 ? '+' : '-', fabs(zOffset));
      else
	// Can not format it centered otherwise; hmm;
	label.Form("0.0");
      
      WriteText(to - TVector2(0.0, 7.0 * etm::cm + dlift), label);
    } //if
  } //for iq
  
  for(unsigned tb=0; tb<2; tb++) {
    if (tb && !mMirrorImage) continue;

    double tbsign = tb ? -1.0 : 1.0;

    for(unsigned lr=0; lr<2; lr++) {
      double lrsign = lr ? 1.0 : -1.0, z0 = lrsign*mIrRegionLength/2;
      
      DrawSingleLine(TVector2(z0, 0.0), TVector2(z0, tbsign*mIrRegionRadius));
      
      for(unsigned iq=0; iq<dim; iq++) {
	double yOffset = step*iq;
	
	if (yOffset > mIrRegionRadius) continue;
	
	TVector2 from(z0, tbsign*yOffset), to(z0 + lrsign*((iq%2) ? 5.0 : 10.0), tbsign*yOffset);
	
	DrawSingleLine(from, to);
      } //for iq
    } //for lr
  } //for tb
} // EicToyModel::DrawAxisTicks()

// ---------------------------------------------------------------------------------------

void EicToyModel::DrawColorLegend(unsigned rows)
{
  if (!rows) rows = 1;

  // FIXME: this was written before EicPalette; merge the two;
  std::map<TString, int> legend;
  // Loop through all registered objects and create a map;
  // No check for duplicates?; well, I call new EtmDetector() myself ...;
  for(auto stack: mStacks) 
    for(auto det: stack->mDetectors)
      if (!mEnabledColorTags.size() || 
	  mEnabledColorTags.find(*det->GetLabel()) != mEnabledColorTags.end())
	// No check for duplicates?; well, I call new EtmDetector() myself ...;
	det->RegisterFillColor(legend);

  {
    unsigned counter = 0;
    double step = _COLOR_LEGEND_STEP_, yOffset0 = mY0 + mYsize - _COLOR_LEGEND_HEIGHT_;
    double zOffset = mX0 + _COLOR_LEGEND_HEIGHT_ + step/2;
    for(auto label: legend) {
      int color = label.second; 
      if (color == _GAP_COLOR_ || label.first == _MARKER_) continue;

      unsigned Xjump = rows == 1 ? 1 : counter%rows, Yjump = rows == 1 ? 0 : counter%rows;
      double yOffset = yOffset0 - Yjump*(_COLOR_LEGEND_HEIGHT_+_COLOR_LEGEND_GAP_);

      TVector2 bl(zOffset - step/2 + _COLOR_LEGEND_GAP_/2, yOffset - _COLOR_LEGEND_HEIGHT_/2);
      TVector2 tr(zOffset + step/2 - _COLOR_LEGEND_GAP_/2, yOffset + _COLOR_LEGEND_HEIGHT_/2); 
      DrawRectangle(bl, tr, color);

      WriteText(TVector2(zOffset, yOffset), label.first.Data(), 
		// FIXME: this is not good, all hardcoded;
		(color == kBlack || color == kBlue+1 || color == kAzure+4) ? kWhite : kBlack);
      
      zOffset += Xjump*step; counter++;
    } //for label
  } //for label
} // EicToyModel::DrawColorLegend()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

ClassImp(EicToyModel)
  

  

  

  
