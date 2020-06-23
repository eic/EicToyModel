
#include <TGeoArb8.h>
#include <TGeoTube.h>
#include <TGeoCone.h>
#include <TGeoCompositeShape.h>

#include <EicToyModel.h>
#include <vc2020_03_20.h>

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

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

vc2020_03_20::vc2020_03_20( void ) 
{ 
  // FIXME: it seems these value will be overwritten every time .root file is imported new?;
  //if (!mBerylliumBeamPipeDiameter) 
  //mBerylliumBeamPipeDiameter = _BERYLLIUM_PIPE_OUTER_DIAMETER_DEFAULT_;
  //printf("%f\n", mHadronBeamPipeOpening);
  if (!mHadronBeamPipeOpening)
    mHadronBeamPipeOpening     = _HADRON_PIPE_OPENING_DEFAULT_;
   
  //CreateGeometry(); 
} // vc2020_03_20::vc2020_03_20()

// ---------------------------------------------------------------------------------------

//double vc2020_03_20::FixedCrossingAngle( void ) const
//{
//return _ESIDE_ALUMINUM_PIPE_DSLOPE_;
//} // vc2020_03_20::FixedCrossingAngle()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void vc2020_03_20::CreateGeometry( void )
{
  // FIXME: well, it's hard to get rid of this; would have to 
  auto eic = EicToyModel::Instance();

  double oslope = mHadronBeamPipeOpening/2, islope = oslope - _HADRON_PIPE_OPENING_DELTA_/2;
  // FIXME: is it correct, or should be 'oslope'?;
  double qslope = islope;

  int green = kGreen+2, cyan = kCyan, yellow = kYellow+1;

  // FIXME: need to clean up in case of a re-creation attempt (say with a different IP shift);
  auto model = GetWorld(); assert(model);
  auto world = model->GetTopVolume(); assert(world);

  // Central portion of the beryllium pipe;
  {
    double length = _ESIDE_BERYLLIUM_PIPE_LENGTH_ + _HSIDE_BERYLLIUM_PIPE_LENGTH_;
    double zOffset = eic->GetIpLocation().X() + 
      (_HSIDE_BERYLLIUM_PIPE_LENGTH_ - _ESIDE_BERYLLIUM_PIPE_LENGTH_)/2;
    
    auto ocpipe = new TGeoTube("BE_PIPE_O",
			       0.0,
			       _BERYLLIUM_PIPE_OUTER_DIAMETER_/2,
			       length/2);
    
    auto vocpipe = new TGeoVolume(ocpipe->GetName(), ocpipe, model->GetMedium("Be"));
    // Strangely enough, SetFillColor() plays no role; 
    vocpipe->SetLineColor(yellow);
    world->AddNode(vocpipe, 0, new TGeoTranslation(0.0, 0.0, zOffset));

    {
      auto icpipe = new TGeoTube("BE_PIPE_I",
				 0.0,
				 _BERYLLIUM_PIPE_INNER_DIAMETER_/2,
				 length/2);
      auto vicpipe = new TGeoVolume(icpipe->GetName(), icpipe, model->GetMedium(_ACCELERATOR_VACUUM_));
      vicpipe->SetLineColor(cyan);
      vocpipe->AddNode(vicpipe, 0, new TGeoTranslation(0.0, 0.0, 0.0));
    }
  }

  {
    // Electron-going direction; 
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
      double zOffset = (z0+z1)/2 -_0_127_, xy[8][2] = {
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
      auto oearb8 = new TGeoArb8("E_PIPE_O", dz10/2, (double*)xy);
      auto voepipe = new TGeoVolume(oearb8->GetName(), oearb8, model->GetMedium("Al"));
      voepipe->SetLineColor(green);
      world->AddNode(voepipe, 0, new TGeoTranslation( 0.0, 0.0, zOffset));
      
      // This is the inner vacuum volume;
      {
	// 'Upstream' in the ROOT coordinate system, where +Z is in hadron-going direction; 
	for(unsigned du=0; du<2; du++) {
	  // FIXME: thickness on the upstream and downstream ends hardcoded;
	  double inwards = du ? _0_127_ : 0.300;
	  
	  for(unsigned ivtx=0; ivtx<4; ivtx++) 
	    for(unsigned ixy=0; ixy<2; ixy++)
	      xy[du*4 + ivtx][ixy] += xy[du*4 + ivtx][ixy] > 0.0 ? -inwards : inwards;
	} //for ud
	
	  // Make the cutting volume a bit longer, in order to be displayed correctly;
	auto iearb8 = new TGeoArb8("E_PIPE_I", (dz10/*+0.01*/)/2, (double*)xy);
	auto viepipe = 
	  new TGeoVolume(iearb8->GetName(), iearb8, model->GetMedium(_ACCELERATOR_VACUUM_));
	viepipe->SetLineColor(cyan);
	voepipe->AddNode(viepipe, 0, new TGeoTranslation( 0.0, 0.0, 0.0));
      }
      
      // Edge plate inbetween the beryllium and aluminum parts (e-side);
#if 1//_TODAY_
      {
	//auto plate  = new TGeoBBox("E_PIPE_CAP_BODY", y0, y0, _0_127_/2);
	auto plate = new TGeoTube("E_PIPE_CAP_BODY", 
				  0.0,
				  _BERYLLIUM_PIPE_OUTER_DIAMETER_/2,
				  _0_127_/2);
	auto vplate = new TGeoVolume(plate->GetName(), plate, model->GetMedium("Al"));
	vplate->SetLineColor(green);
	world->AddNode(vplate, 0, new TGeoTranslation( 0.0, 0.0, z1 - _0_127_/2));
	
	auto hole = new TGeoTube("E_PIPE_CAP_HOLE", 0.0,
				 _BERYLLIUM_PIPE_INNER_DIAMETER_/2,
				 _0_127_/2);
	auto vhole = new TGeoVolume(hole->GetName(), hole, model->GetMedium(_ACCELERATOR_VACUUM_));
	vplate->AddNode(vhole, 0, new TGeoTranslation( 0.0, 0.0, 0.0));
      }
#endif
      // Edge plate inbetween the beryllium and aluminum parts (p-side);
#if 1//_TODAY_
      {
	auto washer = new TGeoTube("P_PIPE_CAP_BODY", 
				   0.0,
				   _HSIDE_ALUMINUM_PIPE_DIAMETER_/2,
				   _0_127_/2);
	auto vwasher = new TGeoVolume(washer->GetName(), washer, model->GetMedium("Al"));
	vwasher->SetLineColor(green);
	world->AddNode(vwasher, 0, new TGeoTranslation( 0.0, 0.0, z2 + _0_127_/2));

	auto hole = new TGeoTube("P_PIPE_CAP_HOLE",
				 0.0,
				 _BERYLLIUM_PIPE_INNER_DIAMETER_/2,
				 _0_127_/2);
	auto vhole = new TGeoVolume(hole->GetName(), hole, model->GetMedium(_ACCELERATOR_VACUUM_));
	vwasher->AddNode(vhole, 0, new TGeoTranslation( 0.0, 0.0, 0.0));
      }
#endif
      
      // Hadron pipe cone; boolean operations on a pair of cones as well as with 
      // TGeoHalfSpace may work, but look totally screwed up in TEve; resort to 
      // define a single hollow cone and cut it by TGeoBBox of appropriate size; 
#if 1//_TODAY_
      {
	// Besides this, ROOT does not seem to like boolean cut of a cone edge 
	// by a plane -> make the cone a bit longer (but with the same opening 
	// angle and wall profile), but cut it at the same z3 location; this muct be correct; 
	double extra = 5.0;
	double length = eic->GetIrRegionLength()/2 - eic->GetIpLocation().X() - _HSIDE_BERYLLIUM_PIPE_LENGTH_;
	// Calculate cone apex distance from z3; 
	double z3 = z2 + _0_127_, apex2z3 = (_HSIDE_ALUMINUM_PIPE_DIAMETER_/2)/tan(oslope);
	double rmax = (apex2z3 + length + extra/2)*tan(oslope);
	
	/*auto ocone =*/ new TGeoCone("H_PIPE_OCONE", (length+extra)/2, 
				      0.0, //(apex2z3          - extra/2)*tan(islope), 
				      (apex2z3          - extra/2)*tan(oslope), 
				      0.0, //(apex2z3 + length + extra/2)*tan(islope), 
				      rmax);
	auto rw = new TGeoRotation();
	rw->RotateY(qslope*180/M_PI); 
	auto trq = new TGeoCombiTrans("TRQ", qslope*length/2, 0.0, z3 + length/2, rw);
	trq->RegisterYourself();
	
	// Has just to be big enough, with the edge at z3;
	new TGeoBBox("HSPACE", 100.0, 100.0, (eic->GetIrRegionLength()/2)/2);
	(new TGeoTranslation("TR0", 0.0, 0.0, z3 - (eic->GetIrRegionLength()/2)/2))->RegisterYourself();
	auto comp = new TGeoCompositeShape("H_PIPE_O", "(H_PIPE_OCONE:TRQ)-(HSPACE:TR0)");
	auto vcone = new TGeoVolume(comp->GetName(), comp, model->GetMedium("Al"));
	//auto vcone = new TGeoVolume(ocone->GetName(), ocone, model->GetMedium("Al"));
	vcone->SetLineColor(green);
	world->AddNode(vcone, 0, new TGeoTranslation( 0.0, 0.0, 0.0));
	//world->AddNode(vcone, 0, trq);

	{
	  /*auto ocone =*/ new TGeoCone("H_PIPE_ICONE", (length+extra)/2, 
					0.0, 
					(apex2z3          - extra/2)*tan(islope), 
					0.0, 
					(apex2z3 + length + extra/2)*tan(islope));
	  auto icomp = new TGeoCompositeShape("H_PIPE_I", "(H_PIPE_ICONE:TRQ)-(HSPACE:TR0)");
	  auto icone = new TGeoVolume(icomp->GetName(), icomp, model->GetMedium(_ACCELERATOR_VACUUM_));
	  vcone->AddNode(icone, 0, new TGeoTranslation( 0.0, 0.0, 0.0));
	}
      }
#endif
    }
  }
} // vc2020_03_20::CreateGeometry()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

ClassImp(vc2020_03_20)
