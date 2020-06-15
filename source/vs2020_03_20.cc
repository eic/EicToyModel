
#include <TGeoArb8.h>
#include <TGeoTube.h>
#include <TGeoCone.h>
#include <TGeoCompositeShape.h>

#include <EicToyModel.h>
#include <vs2020_03_20.h>

// --------------------------------------------------------------------------------------
// These parameters describe the beam pipe layout as of 2020/03/20;
//
#define _BERYLLIUM_PIPE_INNER_DIAMETER_         ( 6.20 * etm::cm)
// "62mm" electron beam pipe has the outer diameter of 63.5mm;
#define _BERYLLIUM_PIPE_OUTER_DIAMETER_         ( 6.35 * etm::cm)

// This beam pipe has a rectangular cross-section;
#define _ESIDE_ALUMINUM_PIPE_HEIGHT_            ( 6.80 * etm::cm)
// Just hardcode these two; 7.3mrad and 25mrad (presumably matching the crossing angle);
#define _ESIDE_ALUMINUM_PIPE_USLOPE_           ( 0.0073)
//#define _ESIDE_ALUMINUM_PIPE_DSLOPE_           ( 0.025)

// Assume the conical beam pipe starts out of this circle; this is actually true;
#define _HSIDE_ALUMINUM_PIPE_DIAMETER_          ( 6.45 * etm::cm)

// Beryllium section is asymmetric with respect to the IP;
#define _ESIDE_BERYLLIUM_PIPE_LENGTH_           (80.00 * etm::cm)
#define _HSIDE_BERYLLIUM_PIPE_LENGTH_           (67.00 * etm::cm)

// In Charlie's model as of 2020/03/20: 0.5mrad "conical thickness";
#define _HADRON_PIPE_OPENING_DELTA_            ( 0.001)

// These 0.127cm (kind of plate thickness) is present in several places -> make a #define; 
#define _0_127_                                ( 0.127 * etm::cm)

// Hadron beam pipe *outer*side* *full* opening; ~51mrad;
#define _HADRON_PIPE_OPENING_DEFAULT_          (  0.051)
// --------------------------------------------------------------------------------------

#ifdef _ETM2GEANT_
//#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
//#include "G4NistManager.hh"
//#include "G4Box.hh"
#include "G4Tubs.hh"
//#include "G4GenericPolycone.hh"
//#include "G4LogicalVolume.hh"
//#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"
#endif

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

vs2020_03_20::vs2020_03_20( void ) 
{ 
  // FIXME: it seems these value will be overwritten every time .root file is imported new?;
  //if (!mBerylliumBeamPipeDiameter) 
  //mBerylliumBeamPipeDiameter = _BERYLLIUM_PIPE_OUTER_DIAMETER_DEFAULT_;
  //printf("%f\n", mHadronBeamPipeOpening);
  if (!mHadronBeamPipeOpening)
    mHadronBeamPipeOpening     = _HADRON_PIPE_OPENING_DEFAULT_;
   
  //CreateGeometry(); 
} // vs2020_03_20::vs2020_03_20()

// ---------------------------------------------------------------------------------------

//double vs2020_03_20::FixedCrossingAngle( void ) const
//{
//return _ESIDE_ALUMINUM_PIPE_DSLOPE_;
//} // vs2020_03_20::FixedCrossingAngle()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void vs2020_03_20::CreateGeometry( void )
{
  // FIXME: well, it's hard to get rid of this; would have to 
  auto eic = EicToyModel::Instance();

  double oslope = mHadronBeamPipeOpening/2, islope = oslope - _HADRON_PIPE_OPENING_DELTA_/2;
  // FIXME: is it correct, or should be 'oslope'?;
  double qslope = islope;

  int green = kGreen+2, cyan = kCyan, yellow = kYellow+1;

  // FIXME: need to clean up in case of a re-creation attempt (say with a different IP shift);
  auto model = /*eic->*/GetWorld(); assert(model);
  auto world = model->GetTopVolume();

  // Central portion of the beryllium pipe;
  {
    double length = _ESIDE_BERYLLIUM_PIPE_LENGTH_ + _HSIDE_BERYLLIUM_PIPE_LENGTH_;
    double zOffset = eic->GetIpLocation().X() + 
      (_HSIDE_BERYLLIUM_PIPE_LENGTH_ - _ESIDE_BERYLLIUM_PIPE_LENGTH_)/2;
    
    auto bpipe = new TGeoTube("BE_PIPE",
			      _BERYLLIUM_PIPE_INNER_DIAMETER_/2,
			      _BERYLLIUM_PIPE_OUTER_DIAMETER_/2,
			      //mBerylliumBeamPipeDiameter/2,
			      length/2);
    
    auto vbpipe = new TGeoVolume(bpipe->GetName(), bpipe, model->GetMedium("Be"));
    // Strangely enough, SetFillColor() plays no role; 
    vbpipe->SetLineColor(yellow);
    world->AddNode(vbpipe, 0, new TGeoTranslation(0.0, 0.0, zOffset));
  }

  {
    // Electron-going direction; 
    {
      // FIXME: unify with the above code later;
      double zr = eic->GetIpLocation().X() - _ESIDE_BERYLLIUM_PIPE_LENGTH_;
      double zl = -eic->GetIrRegionLength()/2, dz = fabs(zr - zl);
      double y0 = _ESIDE_ALUMINUM_PIPE_HEIGHT_/2;
      double dyu = dz*tan(_ESIDE_ALUMINUM_PIPE_USLOPE_);
      //double dyd = dz*tan(_ESIDE_ALUMINUM_PIPE_DSLOPE_);
      double dyd = dz*tan(eic->GetCrossingAngle());//_ESIDE_ALUMINUM_PIPE_DSLOPE_);

      {
	double z1 = eic->GetIpLocation().X() - _ESIDE_BERYLLIUM_PIPE_LENGTH_;
	double z2 = eic->GetIpLocation().X() + _HSIDE_BERYLLIUM_PIPE_LENGTH_;
	double z0 = -eic->GetIrRegionLength()/2, dz10 = fabs(z1 - z0);
	// Do not bother to re-calculate shape precisely; just shift it by the 
	// edge plate thickness;
	double zOffset = (z0+z1)/2 - _0_127_, xy[8][2] = {
	  // Clock-wise; +X is the vertical direction in the detector view TCanvas;
	  { y0+dyu, -y0},
	  {-y0-dyd, -y0},
	  {-y0-dyd,  y0},
	  { y0+dyu,  y0},

	  { y0,     -y0},
	  {-y0,     -y0},
	  {-y0,      y0},
	  { y0,      y0}
	};

	// Looks like TGeoArb8 is the easiest?; this is the outer surface;
	new TGeoArb8("E_PIPE_O", dz10/2, (double*)xy);

	// Carelessly recycle the same array to create edge layer;
	{
	  // In order to see it better :-)
	  double exy[8][2], edz = 1.0;

	  for(unsigned du=0; du<2; du++)
	    for(unsigned ivtx=0; ivtx<4; ivtx++) 
	      for(unsigned ixy=0; ixy<2; ixy++)
		exy[du*4 + ivtx][ixy] = xy[ivtx][ixy];

	  auto edge  = new TGeoArb8("E_PIPE_EDGE", edz/2, (double*)exy);
	  auto vedge = new TGeoVolume(edge->GetName(), edge, model->GetMedium("Edge"));
	  vedge->SetLineColor(cyan);
	  world->AddNode(vedge, 0, new TGeoTranslation( 0.0, 0.0, -eic->GetIrRegionLength()/2 - edz/2 - _0_127_));
	}

	// This is the inner surface and logical subtraction;
	{
	  // 'Upstream' in the ROOT coordinate system, where +Z is in hadron-going direction; 
	  for(unsigned du=0; du<2; du++) {
	    // FIXME: thickness on th eupstream and downstream ends hardcoded;
	    double inwards = du ? _0_127_ : 0.300;
	    
	    for(unsigned ivtx=0; ivtx<4; ivtx++) 
	      for(unsigned ixy=0; ixy<2; ixy++)
		xy[du*4 + ivtx][ixy] += xy[du*4 + ivtx][ixy] > 0.0 ? -inwards : inwards;
	  } //for ud
	  
	  // Make the cutting volume a bit longer, in order to be displayed correctly;
	  new TGeoArb8("E_PIPE_I", (dz10+0.01)/2, (double*)xy);

	  // Edge plate inbetween the beryllium and aluminum parts;
	  auto comp = new TGeoCompositeShape("E_PIPE", "E_PIPE_O-E_PIPE_I");
	  auto vapipe = new TGeoVolume(comp->GetName(), comp, model->GetMedium("Al"));
	  vapipe->SetLineColor(green);
	  world->AddNode(vapipe, 0, new TGeoTranslation( 0.0, 0.0, zOffset));
	}

	// Edge plate inbetween the beryllium and aluminum parts (e-side);
	{
	  new TGeoBBox("E_PIPE_CAP_BODY", y0, y0, _0_127_/2);
	  new TGeoTube("E_PIPE_CAP_HOLE", 0.0,
		       _BERYLLIUM_PIPE_INNER_DIAMETER_/2,
		       _0_127_/2+0.100);
	  auto comp = new TGeoCompositeShape("E_PIPE_CAP", "E_PIPE_CAP_BODY-E_PIPE_CAP_HOLE");
	  auto vacap = new TGeoVolume(comp->GetName(), comp, model->GetMedium("Al"));
	  vacap->SetLineColor(green);
	  world->AddNode(vacap, 0, new TGeoTranslation( 0.0, 0.0, z1 - _0_127_/2));
	}
	// Edge plate inbetween the beryllium and aluminum parts (p-side);
	{
	  auto washer = new TGeoTube("E_PIPE_WASHER", 
				     _BERYLLIUM_PIPE_INNER_DIAMETER_/2,
				     _HSIDE_ALUMINUM_PIPE_DIAMETER_/2,
				     _0_127_/2);
	  auto vwasher = new TGeoVolume(washer->GetName(), washer, model->GetMedium("Al"));
	  vwasher->SetLineColor(green);
	  // On both sides of the beryllium piece;
	  world->AddNode(vwasher, 0, new TGeoTranslation( 0.0, 0.0, z2 + _0_127_/2));
	}

	// Hadron pipe cone; boolean operations on a pair of cones as well as with 
	// TGeoHalfSpace may work, but look totally screwed up in TEve; resort to 
	// define a single hollow cone and cut it by TGeoBBox of appropriate size; 
	{
	  // Besides this, ROOT does not seem to like boolean cut of a cone edge 
	  // by a plane -> make the cone a bit longer (but with the same opening 
	  // angle and wall profile), but cut it at the same z3 location; this muct be correct; 
	  double extra = 5.0;
	  double length = eic->GetIrRegionLength()/2 - eic->GetIpLocation().X() - _HSIDE_BERYLLIUM_PIPE_LENGTH_;
	  // Calculate cone apex distance from z3; 
	  double z3 = z2 + _0_127_, apex2z3 = (_HSIDE_ALUMINUM_PIPE_DIAMETER_/2)/tan(oslope);
	  double rmax = (apex2z3 + length + extra/2)*tan(oslope);

	  new TGeoCone("H_PIPE_CONE", (length+extra)/2, 
		       (apex2z3          - extra/2)*tan(islope), 
		       (apex2z3          - extra/2)*tan(oslope), 
		       (apex2z3 + length + extra/2)*tan(islope), 
		       rmax);
	  auto rw = new TGeoRotation();
	  rw->RotateY(qslope*180/M_PI); 
	  (new TGeoCombiTrans("TRQ", qslope*length/2, 0.0, z3 + length/2, rw))->RegisterYourself();

	  // Has just to be big enough, with the edge at z3;
	  new TGeoBBox("HSPACE", 100.0, 100.0, (eic->GetIrRegionLength()/2)/2);
	  (new TGeoTranslation("TR0", 0.0, 0.0, z3 - (eic->GetIrRegionLength()/2)/2))->RegisterYourself();
	  auto comp = new TGeoCompositeShape("H_PIPE", "(H_PIPE_CONE:TRQ)-(HSPACE:TR0)");
	  auto vcone = new TGeoVolume(comp->GetName(), comp, model->GetMedium("Al"));
	  vcone->SetLineColor(green);
	  world->AddNode(vcone, 0, new TGeoTranslation( 0.0, 0.0, 0.0));

	  {
	    // In order to see it better :-)
	    double thickness = 1.0;
	    auto edge = new TGeoTube("H_PIPE_EDGE", 
				     0.0,
				     rmax,
				     thickness/2);
	    auto vedge = new TGeoVolume(edge->GetName(), edge, model->GetMedium("Edge"));
	    vedge->SetLineColor(cyan);
	    world->AddNode(vedge, 0, 
			   new TGeoCombiTrans(qslope*(length + extra/2 + thickness/2), 0.0, 
					      z3 + (length + extra/2 + thickness/2), rw));
	  }
	}
      }
    }
  } 
} // vs2020_03_20::CreateGeometry()

// ---------------------------------------------------------------------------------------

G4VSolid *vs2020_03_20::CutThisSolid(G4VSolid *solid, const std::vector<TVector2> &polygon)
{
#ifdef _ETM2GEANT_
  //auto eic = EicToyModel::Instance();

  if (!polygon.size()) return 0;

  double cff = cm/etm::cm;

  // Figure out the Z-range;
  std::set<double> zvtx;
  for(auto vtx: polygon)
    zvtx.insert(vtx.X());
  double zmin = *zvtx.begin() - 1E-6, zmax = *zvtx.rbegin() + 1E-6;
  double z0 = (zmin+zmax)/2, zlen = zmax - zmin;

  // FIXME: this needs to be done properly;
  auto tubs = new G4Tubs("Cylinder", 0, 10*cm, cff*zlen, 0, 360.*deg);
  return new G4SubtractionSolid("Diff", solid, tubs, 0, G4ThreeVector(0, 0, cff*z0));
#else
  return 0;
#endif
} // vs2020_03_20::CutThisSolid()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

ClassImp(vs2020_03_20)
