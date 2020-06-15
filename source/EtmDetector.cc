
#include <math.h>

#include <TROOT.h>
#include <TColor.h>

#include <EicToyModel.h>
#include <EtmPalette.h>
#include <EtmDetector.h>

// Some arbitrary number for better visual detector separation; 1cm kind of works;
#define _GAP_WIDTH_         (  1.0)

#ifdef _OPENCASCADE_
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
#endif

#ifdef _ETM2GEANT_
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4GenericPolycone.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"
#endif

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

EtmDetector::EtmDetector(const EtmDetectorStack *stack, const char *label, int fillcolor, 
			 int linecolor, double length): 
  EtmPolygonGroup(fillcolor, linecolor),
  mStack(stack), //mSegmentation(1), 
  mLength(length), mOffset(0.0), mActualDistance(0.0)
{
  trim(_DEFAULT_TRIM_VALUE_);

  mLabel = label ? new TString(label) : 0;
} // EtmDetector::EtmDetector()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------

void EtmDetector::ClearPolygonBuffer( void )
{
  ClearPolygons();
  //@@@for(auto &pgroup: _mPolygonGroups)
  //@@@pgroup->Clear();

  //@@@for(auto child: mChildren)
  //@@@child->ClearPolygonBuffer();
} // EtmDetector::ClearPolygonBuffer()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

EtmPolygonGroup *EtmDetector::insert(const char *tag, int linecolor)
{
  auto eic = EicToyModel::Instance();
  int fillcolor = eic->Palette().GetColor(tag);
  if (fillcolor == -1) {
    printf("Tag '%s' does not exist!\n", tag);
    return 0;
  } //if

  auto group = new EtmPolygonGroup(fillcolor, linecolor);
  mGraphicsElements.push_back(group);

  return group;
} // EtmDetector::insert()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void EtmDetector::RegisterFillColor(std::map<TString, int> &legend)
{
  const TString *label = GetLabel();

  if (label && !label->IsNull())
    legend[*label] = GetFillColor();

  //for(auto group: mGraphicsElements)
  //group->RegisterFillColor(legend);
  //@@@for(auto child: mChildren)
  //@@@child->RegisterFillColor(legend);
} // EtmDetector::RegisterFillColor()

// ---------------------------------------------------------------------------------------

EtmDetector *EtmDetector::trim(double etamin, double etamax, bool preserve_modified) 
{
  // Sanity fixes first;
  if (etamin < 0.0) etamin = 0.0; if (etamin > 1.0) etamin = 1.0; 
  if (etamax < 0.0) etamax = 0.0; if (etamax > 1.0) etamax = 1.0; 

  if (!preserve_modified || mEdgeTrimming[0] == _DEFAULT_TRIM_VALUE_)
    mEdgeTrimming[0] = etamin; 
  if (!preserve_modified || mEdgeTrimming[1] == _DEFAULT_TRIM_VALUE_)
    mEdgeTrimming[1] = etamax; 

  return this;
} // EtmDetector::trim()

// ---------------------------------------------------------------------------------------

EtmDetector *EtmDetector::stretch(EtmDetector *refdet, double toffset, etm::Stretch how)
{
  auto eic = EicToyModel::Instance();

  if (refdet != mStack->get(0)) {
    // A clear mistake on user side -> do nothing; FIXME: warning, please;
    if (mStack == refdet->mStack) return this;

    // This also should not happen; FIXME: issue a warning;
    if (mStack != eic->mid() && refdet->mStack != eic->mid() && 
	!(mStack == eic->vtx() && refdet->mStack != eic->mid())) 
      return this;

    eic->AddBoundaryModificationRequest(this, refdet, toffset, how);
  } else {
    // Reference detector is not specified; one can try to guess;
    if (mStack == eic->mid() || mStack == eic->vtx()) {
      eic->AddBoundaryModificationRequest(this, eic->bck()->get(0), toffset, how);
      eic->AddBoundaryModificationRequest(this, eic->fwd()->get(0), toffset, how);
    }
    else
      eic->AddBoundaryModificationRequest(this, eic->mid()->get(0), toffset, how);
  } //if

  return this;
} // EtmDetector::stretch()

// ---------------------------------------------------------------------------------------

// FIXME: well, this is crappy way of coding: give mDummyDetector of either my own stack
// or of some other stack in order to indicate a necessity of a special treatment;

EtmDetector *EtmDetector::stretch(                            double tlength, etm::Stretch how) 
{
  return stretch(mStack->get(0), tlength, how);
} // EtmDetector::stretch()

EtmDetector *EtmDetector::stretch(EtmDetectorStack *refstack, double tlength, etm::Stretch how) 
{
  return stretch(refstack->get(0), tlength, how);
} // EtmDetector::stretch()

// ---------------------------------------------------------------------------------------

void EtmDetector::Build( void )
{
  auto eic = EicToyModel::Instance();
  TVector2 ip = eic->GetIpLocation();

  ClearPolygonBuffer();

  // If color=0, it is a space holder -> skip the actual drawing;
  if (GetFillColor() == _GAP_COLOR_ || *GetLabel() == _MARKER_) return;
 
  //if (!ConditionChecker(distance - mLength/2 >= 0.0, 
  //			  "Not enough space for detector '%s'", GetLabel()->Data()))
  //return 0.0;
  
  {
    //if (*GetLabel() == _MARKER_) return;// 0.0;//-eic->SafetyClearance();
    std::vector<TVector2> sides[2], vertices;
    //+++double dgap = eic->VisualClearance() - eic->SafetyClearance();
    
    for(unsigned vh=0; vh<2; vh++) 
      for(unsigned tb=0; tb<2; tb++) {
	for(unsigned bf=0; bf<2; bf++) {
	  auto boundary = mStack->EtaBoundary(bf);
	  double sign = bf == tb ? -1.0 : 1.0;
	  auto &vtxarr = boundary->mCrackNodes[vh][tb];
	  //std::vector<TVector2> qvtxarr;

	  TVector2 axis(mStack->AlignmentAxis().X(), (tb ? -1.0 : 1.0)*mStack->AlignmentAxis().Y());

	  {
#if _LATER_
	    std::vector<EtmLine2D> lines;
	    for(unsigned ivtx=1; ivtx<vtxarr.size(); ivtx++) 
	      lines.push_back(EtmLine2D(vtxarr[ivtx-1], (vtxarr[ivtx]-vtxarr[ivtx-1]).Unit()));
	    
	    for(unsigned iln=0; iln<lines.size(); iln++) {
	      auto &line = lines[iln];
	      
	      // FIXME: need to account for crack width here!;
	      //if (line.N().X() && line.N().Y())
	      line.Shift(sign*(eic->SafetyClearance()/2)*line.N());
	    } //for iln 
	    
	    qvtxarr.push_back(lines[0].X());
	    //for(unsigned iln=1; iln<lines.size(); iln++) 
	    //assert(!lines[iln-1].IsParallel(lines[iln]));
	    for(unsigned iln=1; iln<lines.size(); iln++) 
	      if (!lines[iln-1].IsParallel(lines[iln]))
		qvtxarr.push_back(lines[iln-1].Cross(lines[iln]));
	    // FIXME: come on!;
	    qvtxarr.push_back(vtxarr[vtxarr.size()-1] + 
			      (sign*eic->SafetyClearance()/2)*lines[lines.size()-1].N());
#endif
	      
	    // FIXME: 1E-6 stuff;
	    auto from = EtmLine2D(ip + (mActualDistance - length()/2 + 1E-6/* + dgap/2*/)*axis, 
				  axis.Rotate(-M_PI/2));
	    auto to   = EtmLine2D(ip + (mActualDistance + length()/2 - 1E-6/* - dgap/2*/)*axis, 
				  axis.Rotate(-M_PI/2));
	    
	    //sides[mm] = eic->Band(qvtxarr, from, to);
	    sides[bf] = eic->Band( vtxarr, from, to);
	    
	    {
	      double trim = mEdgeTrimming[bf];
	      // FIXME: do it better (high |eta| edges are on the "wrong side" from 
	      // the alignment axis);
	      if (mStack == eic->bck() && !bf && trim != 0.0 && trim != 1.0) trim = 1.0;
	      if (mStack == eic->fwd() &&  bf && trim != 0.0 && trim != 1.0) trim = 1.0;
		
	      // Otherwise no need to do anything (full edge fill);
	      if (trim < 1.0) {
		double dmin = 0.0, dmax = 0.0;
		// FIXME: bottom half does not work well;
		EtmLine2D thrust(ip, axis);
		
		for(unsigned ivtx=0; ivtx<sides[bf].size(); ivtx++) {
		  auto vtx = sides[bf][ivtx];
		  double d = thrust.Distance(vtx);
		  if (!ivtx || d < dmin) dmin = d;
		  if (!ivtx || d > dmax) dmax = d;
		} //for vtx
		
		  // FIXME: well, there whould be some floating point precision cutoff 
		  // indicating that say two lines are parallel (even if their normal 
		  // vectores are not exactly equal, as calculated);
		if (fabs(dmax - dmin) > 1E-6) {
		  //double range = (dmax - dmin)*trim, limit = mm ? dmax - range : dmin + range;
		  double range = (dmax - dmin)*(tb ? 1.0 - trim : trim), limit = bf ? dmax - range : dmin + range;
		  
		  if (trim) {
		    EtmLine2D cut(ip + limit*thrust.N(), -sign*axis);
		    //EtmLine2D cut(ip + limit*thrust.N(), -1*axis);
		    
		    auto halves = EicToyModel::GetIntersection(sides[bf], cut);
		    
		    sides[bf].clear();
		    for(auto &vtx: halves.first)
		      sides[bf].push_back(vtx);
		    sides[bf].push_back(to.Cross(cut));
		  } else {
		    for(auto &vtx: sides[bf]) {
		      double d = thrust.Distance(vtx);
		      TVector2 along = vtx - d*thrust.N();
		      
		      vtx = along + limit*thrust.N();
		    } //for vtx
		  } //if
		} //if
	      } //if
	    }
	  }
	} //for bf

	{
	  // FIXME: optimize;
	  vertices = sides[0];
	  
	  for(unsigned ivtx=0; ivtx<sides[1].size(); ivtx++)
	    vertices.push_back(sides[1][sides[1].size()-1-ivtx]);
	  
	  //printf("%s -> %d %d %d\n", mLabel.Data(), sides[0].size(), sides[1].size(), vertices.size());
	  AddPolygon(vertices);
	}
      } //for tb
    }

  // If a child detector exists, build it as well, recursively; not interested 
  // in the return value for the children;
  //for(auto child: mChildren)
  //child->Build(ip, axis, distance + child->mOffset, boundaries, tb, false);
} // EtmDetector::Build()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

G4VPhysicalVolume *EtmDetector::PlaceG4Volume(G4LogicalVolume *world, const char *name)
{
  // Can be a GAP or a MARKER detector;
  if (!Polygons().size()) return 0;

#ifdef _ETM2GEANT_
  // Air, what else can it be?;
  auto air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  auto polygon = Polygons()[0];
  unsigned dim = polygon.size();
  double cff = cm/etm::cm, z[dim], r[dim];

  auto eic = EicToyModel::Instance();

  // Treat Z-offsets differently for central/vertex and endcap detectors;
  TVector2 ip = eic->GetIpLocation();
  double z0 = (mStack == eic->bck() || mStack == eic->fwd()) ? 
    (ip + mActualDistance*mStack->AlignmentAxis()).X() : ip.X();
  // FIXME: make it optional;
  z0 = 0.0;
  
  for(unsigned ivtx=0; ivtx<dim; ivtx++) {
    auto &pt = polygon[ivtx]; 

    z[ivtx] = (pt.X() - z0)*cff; 
    r[ivtx] = pt.Y()*cff; 
    
    //printf("%7.2f %7.2f\n", pt.X(), pt.Y());
  } //for vtx

  // FIXME: take order (like TRD 0/1/2) into account as well;
  TString label = name ? TString(name) : (mStack->GetLabel() + "." + *GetLabel());
  
  // For now just a generic polycone; FIXME: implement G4BREPSolidPolyhedra later;
  auto vpol = new G4GenericPolycone(label.Data(), 0., 360.*deg, dim, r, z);

  // Extract ROOT color attributes and assign them to GEANT volumes;
  auto rcolor = gROOT->GetColor(GetFillColor());
  G4VisAttributes* visAtt = 
    new G4VisAttributes(G4Colour(rcolor->GetRed(), rcolor->GetGreen(), rcolor->GetBlue()));
  visAtt->SetVisibility(true);
  visAtt->SetForceWireframe(false);
  visAtt->SetForceSolid(true);

  {
    auto vs = eic->GetVacuumSystem();
    bool vs_cut_required = vs;

    // Figure out whether a cut is needed at all; these are mutually exclusive of 
    // course; fine, no staggered elseif's;
    if (mStack == eic->vtx() && eic->vtx()->GetDetector(0) != this) vs_cut_required = false;
    if (mStack == eic->mid() && 
	// Either vertex stack is populated or this is not the first central stack 
	// detector -> under no sane configuration it can sit next to the beam pipe;
	(eic->vtx()->DetectorCount() || (eic->mid()->GetDetector(0) != this)))
      vs_cut_required = false;

    // Vacuum system defined -> carve a hole in the original poly-shape;
    auto vcut = vs && vs_cut_required ? vs->CutThisSolid(vpol, polygon) : 0;

    if (vcut) {
      auto lcut = new G4LogicalVolume(vcut, air, label.Data(), 0, 0, 0);
      lcut->SetVisAttributes(visAtt);

      return new G4PVPlacement(0, G4ThreeVector(0, 0, z0*cff), lcut, label.Data(), world, false, 0);
    } else {
      // No vacuum system defined -> a simple volume export;
      auto lpol = new G4LogicalVolume(vpol, air, label.Data(), 0, 0, 0);
      lpol->SetVisAttributes(visAtt);
      
      return new G4PVPlacement(0, G4ThreeVector(0, 0, z0*cff), lpol, label.Data(), world, false, 0);
    } //if
  }
#else
  return 0;
#endif
} // EtmDetector::PlaceG4Volume()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void EtmDetector::Export(const char *fname)
{
#ifdef _OPENCASCADE_
  // Can be a GAP or a MARKER detector;
  if (!Polygons().size()) return;

  auto polygon = Polygons()[0];
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

    poly.Add(gp_Pnt((pt.X()-z0)*cff, 0.0, pt.Y()*cff));

    //z[ivtx] = (pt.X() - z0)*cff; 
    //r[ivtx] = pt.Y()*cff; 
    
    //printf("%7.2f %7.2f\n", pt.X(), pt.Y());
  } //for vtx
  poly.Close();

  // Build a 2D face out of it; then create a revolution body;
  auto face = BRepBuilderAPI_MakeFace(poly);
  gp_Ax1 axis(gp_Pnt(0,0,0), gp_Dir(0,0,1)); 
  auto solid = BRepPrimAPI_MakeRevol(face, axis); 

  // Create XCAF document;
  Handle(TDocStd_Document) outDoc;
  Handle(XCAFApp_Application) outApp = XCAFApp_Application::GetApplication(); 
  outApp->NewDocument("ETM", outDoc); 
  
  Handle (XCAFDoc_ShapeTool) outAssembly = XCAFDoc_DocumentTool::ShapeTool (outDoc->Main()); 
  
  Handle(XCAFDoc_ColorTool) outColors = XCAFDoc_DocumentTool::ColorTool(outDoc->Main()); 

  // Assign ROOT TGeo color;
  {
    auto rcolor = gROOT->GetColor(GetFillColor());
    
    Quantity_Color ccolor(rcolor->GetRed(), rcolor->GetGreen(), rcolor->GetBlue(), Quantity_TOC_RGB);
    TDF_Label aLabel = outAssembly->AddShape(solid);
    outColors->SetColor(aLabel, ccolor, XCAFDoc_ColorSurf);
  }

  // Write the file out;
  {
    STEPCAFControl_Writer cWriter;
    
    cWriter.Transfer(outDoc, STEPControl_ManifoldSolidBrep);
    //cWriter.Transfer(outDoc, STEPControl_AsIs);
    cWriter.Write(fname);
  }
#endif
} // EtmDetector::Export()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

ClassImp(EtmDetector)
