//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "G4Box.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"

#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

#include <EicToyModel.h>
#include "HCalDetectorConstruction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HCalDetectorConstruction* HCalDetectorConstruction::mInstance = 0;

HCalDetectorConstruction::HCalDetectorConstruction(): G4VUserDetectorConstruction()
{
  fExpHall_x = fExpHall_y = fExpHall_z = 200.0*cm;

  mInstance = this;
} // HCalDetectorConstruction::HCalDetectorConstruction()

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HCalDetectorConstruction::~HCalDetectorConstruction(){;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double gPhotonEnergy[_GDIM_];

const double SmallGap    =    0.02*mm; 

// Assume Hamamatsu 3x3mm^2 sensors;
static double SiPMSize              =    3.0*mm;
// FIXME: this is all fake; 
static double SiPMThickness         =    1.0*mm;
static double SensorThickness       =    0.1*mm;

// -------------------------------------------------------------------------------------

#ifdef _USE_ZEBRA_FILTER_
struct HcalMaskLine {
  // NB: Y-offset is counter from the middle;
  double y0, width, length;
};

// NB: and the other Y-symmetric 21 line;
#define _HCAL_MASK_LINE_NUM_ (21+1)//(2*21+1)

static HcalMaskLine HcalMaskLines[_HCAL_MASK_LINE_NUM_] = {
  // This is assumed to be the central line;
  {  0.00, 1.35,  588.1},

  // Type in everything explicitely (Y-offsets are irregular);
  {  2.25, 1.35, 168.9},
  {  4.50, 1.35, 378.5},
  {  6.75, 1.35, 168.9},
  {  9.00, 1.00, 561.9},
  { 10.90, 1.35, 247.5},
  { 13.15, 1.35, 457.1},
  { 15.40, 1.35, 247.5},
  { 17.65, 1.35, 588.1},
  { 19.90, 1.35, 247.5},
  { 22.15, 1.35,  90.3},
  { 24.40, 1.35, 457.1},
  { 26.65, 1.35, 326.1},
  { 28.90, 1.35,  90.3},
  { 31.15, 1.00, 561.9},
  { 33.05, 1.35, 326.1},
  { 35.30, 1.35,  90.3},
  { 37.55, 1.35, 483.3},
  { 39.80, 1.35, 378.5},
  { 42.05, 1.35, 168.9},
  { 44.30, 1.35, 588.1},
  { 48.00, 0.25, 840.0}};
#endif

// -------------------------------------------------------------------------------------

//#include <G4GDMLParser.hh>

G4VPhysicalVolume *HCalDetectorConstruction::DefineHCalVolumes( void )
{
#if 0
  {
    // The experimental Hall;
    G4Box* expHall_box = new G4Box("World", fExpHall_x/2, fExpHall_y/2, fExpHall_z/2);
    G4LogicalVolume* expHall_log = new G4LogicalVolume(expHall_box, mQair, "World", 0, 0, 0);
    G4VPhysicalVolume* expHall_phys = 
      new G4PVPlacement(0, G4ThreeVector(), expHall_log, "World", 0, false, 0);

    G4GDMLParser parser;
    parser.Read("block.gdml");
    //parser.ReadModule("00100.gdml");
    //G4VPhysicalVolume *gdml = parser.GetWorldVolume();
    G4LogicalVolume *gdml = parser.GetVolume("V-Product");
    //G4LogicalVolume *gdml = parser.GetVolume("V-Open CASCADE STEP translator 6.7 110");//"V-Product");//GetWorldVolume();
    //G4LogicalVolume *gdml = parser.GetVolume("V-Open CASCADE STEP translator 6.7 101");//"V-Product");//GetWorldVolume();
    //gdml->SetVisAttributes(G4VisAttributes::Invisible);
    //gdml->GetLogicalVolume()->SetVisAttributes(G4VisAttributes::Invisible);
    
    // Offset it more or less in sync with the beam;
    new G4PVPlacement(0, G4ThreeVector(1.01*cm, 1.01*cm, 0), gdml, "V-Product", expHall_log, false, 0);
    //new G4PVPlacement(0, G4ThreeVector(1.01*cm, 7.01*cm, 0), gdml, "V-Product", expHall_log, false, 0);
    //new G4PVPlacement(0, G4ThreeVector(3.7*m, 0, 0), gdml, "V-Product", expHall_log, false, 0);

    // return gdml;//expHall_phys;
    return expHall_phys;
  }
#endif

  double TowerWidth                 =  100.0*mm;
  double TowerHeight                =  100.0*mm;
  //double SteelSpacerThickness       =    2.0*mm;

  // Absorber+scintillator building blocks; 
  //double SubCellLength              =   13.1*mm;
  //const unsigned SubCellNum         =     64;
  double SubCellLength              =   23.4*mm;
  // FIXME: the last 20mm steel layer problem;
  const unsigned SubCellNum         =     37;
  // FIXME: assume all 64 cells have Pb absorber (in reality the last one was made of steel);
  // NB: want to have some extra length for sensors; 
  double TowerLength                = SubCellNum * SubCellLength + 10.0*mm;
  //printf("Essential tower length: %7.2f\n", SubCellNum * SubCellLength); 

  double TowerEnvelopeWidth         = TowerWidth  - 2*SmallGap;
  double TowerEnvelopeHeight        = TowerHeight - 2*SmallGap - mGeometry->mSteelSpacerThickness;
  double TowerEnvelopeLength        = TowerLength - 2*SmallGap;

  double SubCellWidth               =   96.0*mm;
  double SubCellHeight              = TowerEnvelopeHeight - 2*SmallGap;

  double SubCellEnvelopeWidth       = SubCellWidth  - 2*SmallGap;
  double SubCellEnvelopeHeight      = SubCellHeight - 2*SmallGap;
  double SubCellEnvelopeLength      = SubCellLength - 2*SmallGap;

  double AbsorberPlateWidth         = SubCellWidth  - 2*SmallGap;
  double AbsorberPlateHeight        = SubCellHeight - 2*SmallGap;
  // FIXME: ignore paint layer; just give absorber plates some 
  // optical properties (diffuse reflection, etc);
  double AbsorberPlateThickness     =   20.0*mm;
  //double AbsorberPlateThickness     =   10.0*mm;
  //@@@double AbsorberPlateThickness     =    7.5*mm;
  //double AbsorberPlateThickness     =    5.0*mm;

  // Scintillator parameters; assume they are for sure smaller than subcell XY-size;
  double ScintillatorPlateThickness =    3.0*mm;
  //double ScintillatorPlateThickness =    2.5*mm;
  //@@@double ScintillatorPlateThickness =    5.0*mm;
  //double ScintillatorPlateThickness =    7.5*mm;
  double ScintillatorPlateWidth     =   95.0*mm;
  double ScintillatorPlateHeight    =   97.0*mm;
  double ScPlateAirAppendixLength   =    0.3*mm;

  // WLS parameters; 
  double WlsPlateThickness          =    3.0*mm;
  double WlsPlateLength             = SubCellNum * SubCellLength;
  double WlsPlateHeight             =   97.0*mm;
  // Artificial air volume; its thickness does not really matter;
  double WlsPlateAirAppendixLength  =    1.0*mm;


#if _QQQ_
  // Let it be 100um; does not really matter as long as all this crap fits into 100mm wide tower;
  double MetallizedMylarThickness   =    0.05*mm;
#ifdef _USE_ZEBRA_FILTER_
  double ZebraFilterThickness       =    0.05*mm;
#endif

  double OpticalGlueThickness       =    _HCAL_OPTICAL_GLUE_THICKNESS_;
#endif

  auto eic = EicToyModel::Instance();

  // Print IR dimensions;
  printf("\n\nIR box size: Z +/- %.2f [cm], R ~ %.2f [cm]\n\n", 
	 eic->GetIrRegionLength()/2, eic->GetIrRegionRadius());
  
  // The easiest: ask the model to build its IR world;
  auto expHall_phys = eic->ConstructG4World();
  expHall_phys->GetLogicalVolume()->SetVisAttributes(G4VisAttributes::Invisible);


  
  // Construct the integration volumes geometry, internally;
  TFile fin(argv[1]);
  dynamic_cast<EicToyModel *>(fin.Get("EicToyModel"));
  eic->Construct();
  eic->PlaceG4Volumes(expHall_phys);
  
  // Place "MyHCal" tower matrix into the integration volume bubble instead of the world; 
  new G4PVPlacement(0, G4ThreeVector(0, 0, zOffset), myhcal_log, "MyHCal", expHall_log,     false, 0);
  auto hcal_bubble_log = eic->fwd()->get("HCal")->GetG4Volume()->GetLogicalVolume();
  new G4PVPlacement(0, G4ThreeVector(0, 0,       0), myhcal_log, "MyHCal", hcal_bubble_log, false, 0);
  


  //auto expHall_log = expHall_phys->GetLogicalVolume();
  auto expHall_log = eic->bck()->get("HCal")->GetG4Volume()->GetLogicalVolume();

  // The experimental Hall;
  //G4Box* expHall_box = new G4Box("World", fExpHall_x/2, fExpHall_y/2, fExpHall_z/2);
  //G4LogicalVolume* expHall_log = new G4LogicalVolume(expHall_box, mQair, "World", 0, 0, 0);
  //G4VPhysicalVolume* expHall_phys = 
    // new G4PVPlacement(0, G4ThreeVector(), expHall_log, "World", 0, false, 0);
  expHall_phys->GetLogicalVolume()->SetVisAttributes(G4VisAttributes::Invisible);

  ///*auto emcal =*/ eic->bck()->get("HCal")->PlaceG4Volume(expHall_phys);

  // Front steel plate; assume rectangular box, wedge shape is an overkill here;
  if (mGeometry->mUseFrontSteelPlate) {
    G4Box *front_plate_box = 
      new G4Box("FrontPlate", (mGeometry->mXdim*TowerWidth)/2, (mGeometry->mYdim*TowerHeight)/2, 
		mGeometry->mFrontSteelPlateThickness/2);
    G4LogicalVolume *front_plate_log = new G4LogicalVolume(front_plate_box, mSteel, "FrontPlate", 0, 0, 0);
    {
      double zOffset = -(TowerLength + mGeometry->mFrontSteelPlateThickness)/2;
      
      new G4PVPlacement(0, G4ThreeVector(0, 0, zOffset), 
			front_plate_log, "FrontPlate", expHall_log, false, 0);
    }
  } //if

  // The "large" (tower matrix) periodic part; 
  G4Box *tower_box = new G4Box("Tower", TowerWidth/2, TowerHeight/2, TowerLength/2);
  // NB: assume towers are optically isolated; make them out of "air without optical 
  // properties"; NB: inner envelopes will be filled with regular air (so light can travel inside);
  G4LogicalVolume *tower_log = new G4LogicalVolume(tower_box, mQair, "Tower", 0, 0, 0);
  for(unsigned ix=0; ix<mGeometry->mXdim; ix++) 
    for(unsigned iy=0; iy<mGeometry->mYdim; iy++) {
      double xOffset = (ix-(mGeometry->mXdim-1)/2.0)*TowerWidth;
      double yOffset = (iy-(mGeometry->mYdim-1)/2.0)*TowerHeight;

      //printf("%2d %2d\n", ix, iy);
      //++new G4PVPlacement(0, G4ThreeVector(xOffset, yOffset, 0),
      new G4PVPlacement(0, G4ThreeVector(xOffset + 50*cm, yOffset - 0*cm, 0), 
			tower_log, "Tower", expHall_log, false, ix*mGeometry->mYdim + iy);
    } //for ix..iy

#if _TODAY_
  // Tower inner air envelope; see comment for SubCell envelope below;
  G4Box *tower_envelope_box = 
    new G4Box("TowerEnvelope", TowerEnvelopeWidth/2, TowerEnvelopeHeight/2, TowerEnvelopeLength/2);
  G4LogicalVolume *tower_envelope_log = 
    new G4LogicalVolume(tower_envelope_box, mAir, "TowerEnvelope", 0, 0, 0);
#ifdef _HCAL_USE_MYLAR_REFLECTOR_
  G4VPhysicalVolume *tower_envelope_phys = 0;
#endif
  {
    double yOffset = -TowerHeight/2 + SmallGap + TowerEnvelopeHeight/2;
#ifdef _HCAL_USE_MYLAR_REFLECTOR_
    tower_envelope_phys = 
#endif
      new G4PVPlacement(0, G4ThreeVector(0, yOffset, 0), tower_envelope_log, "TowerEnvelope", tower_log, false, 0);
  }

  // The "small" periodic part; assume dimensions defined by lead/antimony plate XY-size; 
  {
    G4Box *subcell_box = new G4Box("SubCell", SubCellWidth/2, SubCellHeight/2, SubCellLength/2);
    G4LogicalVolume *subcell_log = new G4LogicalVolume(subcell_box, mAir, "SubCell", 0, 0, 0);
    {   
      // double xOffset = -TowerEnvelopeWidth/2 + SmallGap + SubCellEnvelopeWidth/2;
      double xOffset = -TowerEnvelopeWidth/2 + SmallGap + SubCellWidth/2;
      
      for(unsigned iq=0; iq<SubCellNum; iq++) {
	double zOffset = (iq-(SubCellNum-1)/2.0)*SubCellLength;
	
	/*if (!iq || iq == 32 || iq == SubCellNum-1)*/ {
	  //printf("@@@ %7.2f\n", zOffset);
	  new G4PVPlacement(0, G4ThreeVector(xOffset, 0, zOffset), subcell_log, 
			    "SubCell", tower_envelope_log, false, iq);
	} //if
      } //for iq
    }
  
    // Inner air envelope volume; need it in order to define surfaces like air-to-absorber 
    // for a single subcell physical volume rather than to all its copies;
    G4Box *subcell_envelope_box = 
      new G4Box("SubCellEnvelope", SubCellEnvelopeWidth/2, SubCellEnvelopeHeight/2, SubCellEnvelopeLength/2);
    G4LogicalVolume *subcell_envelope_log = 
      new G4LogicalVolume(subcell_envelope_box, mAir, "SubCellEnvelope", 0, 0, 0);
    G4VPhysicalVolume *subcell_envelope_phys = 
      new G4PVPlacement(0, G4ThreeVector(), subcell_envelope_log, "SubCellEnvelope", subcell_log, false, 0);

    // Scintillator plate;
    {
      G4Box *scintPlate_box = new G4Box("ScintPlate", ScintillatorPlateWidth/2, ScintillatorPlateHeight/2, 
					ScintillatorPlateThickness/2);
      G4LogicalVolume *scintPlate_log = new G4LogicalVolume(scintPlate_box, mEJ212, "ScintPlate", 0, 0, 0);
      
      // Assume plate is shifted to one side of the air envelope, away from the WLS (worst case);
      double xOffset = -SubCellEnvelopeWidth /2 + SmallGap + ScintillatorPlateWidth    /2 + ScPlateAirAppendixLength;
      double yOffset = -SubCellEnvelopeHeight/2 + SmallGap + ScintillatorPlateHeight   /2;
      double zOffset = -SubCellEnvelopeLength/2 + SmallGap + ScintillatorPlateThickness/2;
      
      G4VPhysicalVolume *sc_phys = 
	new G4PVPlacement(0, G4ThreeVector(xOffset, yOffset, zOffset), scintPlate_log, "ScintPlate", 
			  subcell_envelope_log, false, 0);
	  
      // Then add a small "piece of air" next to the far-from-WLS side and define a diffusive paint;
      // FIXME: unify with the WLS code;
      {
	double xxOffset = -SubCellEnvelopeWidth /2 + SmallGap + ScPlateAirAppendixLength/2;

	G4Box *xx_box = new G4Box("XxPlate", ScPlateAirAppendixLength/2, ScintillatorPlateHeight/2, 
				  ScintillatorPlateThickness/2);
	G4LogicalVolume *xx_log = new G4LogicalVolume(xx_box, mAir, "XxPlate", 0, 0, 0);
	G4PVPlacement *xx_phys = 
	  new G4PVPlacement(0, G4ThreeVector(xxOffset, yOffset, zOffset),
			    xx_log, "XxPlate", subcell_envelope_log, false, 0);
	
	// NB: physical surface interface seems to have precedence over logical ones, so this 
	// does not disturb the other 5 facets (pretty much exactly what I need);
	G4OpticalSurface* opScPaintSurface = 
	  CreateLambertianSurface("ScPaintSurface", _BC620_PAINT_REFLECTIVITY_);
	new G4LogicalBorderSurface("ScPaintAirSurface", sc_phys, xx_phys, opScPaintSurface);
	new G4LogicalBorderSurface("AirPaintScSurface", xx_phys, sc_phys, opScPaintSurface);
      }
      
      // NB: if 100% reflectivity and ideally polished surface is selected, no reason to overcomplicate 
      // things -> just let GEANT use material refractive indices;
      if (_EJ212_SCINTILLATOR_SURFACE_REFLECTIVITY_ != 1.00 || _EJ212_SCINTILLATOR_SURFACE_ROUGHNESS_) 
	DefineSkinSurface("ScintSurface", scintPlate_log, _EJ212_SCINTILLATOR_SURFACE_REFLECTIVITY_, 
			  _EJ212_SCINTILLATOR_SURFACE_ROUGHNESS_);
    }

    // The absorber (lead+antimony) plate;
    {
      G4Box* absorber_box = 
	new G4Box("Absorber", AbsorberPlateWidth/2, AbsorberPlateHeight/2, AbsorberPlateThickness/2);
#ifdef _USE_STEEL_ABSORBER_
      G4LogicalVolume* absorber_log = new G4LogicalVolume(absorber_box, mSteel, "Absorber", 0, 0, 0);
#else
      G4LogicalVolume* absorber_log = new G4LogicalVolume(absorber_box, mAbsorber, "Absorber", 0, 0, 0);
#endif
      
      {
	double zOffset = SubCellEnvelopeLength/2 - SmallGap - AbsorberPlateThickness/2;
	
	G4VPhysicalVolume* absorber_phys =
	  new G4PVPlacement(0, G4ThreeVector(0, 0, zOffset), absorber_log, "Absorber",
			    subcell_envelope_log, false, 0);
	
	// NB: don't touch anything here without a thorough check!; here play a bit different game, 
	// than with the scintillator; care only about photons in air->absorber direction and force
	// pure diffuse (Lambertian) reflection; the easiest way to get this is to use 
	// 'groundfrontpainted' surface finish;
	G4OpticalSurface* opAbsorberSurface = 
	  CreateLambertianSurface("AbsorberSurface", _ABSORBER_PAINT_REFLECTIVITY_);
	new G4LogicalBorderSurface("AbsorberSurface", subcell_envelope_phys, absorber_phys, opAbsorberSurface);
      }
    }
  }

  // WLS volume;
  double wlsXoffset = TowerEnvelopeWidth/2 - WlsPlateThickness/2 - SmallGap;
  double wlsYoffset = -TowerEnvelopeHeight/2 + SmallGap + WlsPlateHeight/2;
  {
    G4Box *wls_box = new G4Box("WlsPlate", WlsPlateThickness/2, WlsPlateHeight/2, WlsPlateLength/2);
    G4LogicalVolume *wls_log = new G4LogicalVolume(wls_box, mEJ280, "WlsPlate", 0, 0, 0);

    G4VPhysicalVolume *wls_phys = 
      new G4PVPlacement(0, G4ThreeVector(wlsXoffset, wlsYoffset, 0), wls_log, "WlsPlate", tower_envelope_log, false, 0);

    // Same logic as with the scintillator plate: fist make all 6 facets "almost 100%" reflecting;
    if (_WLS_SURFACE_REFLECTIVITY_ != 1.00 || _WLS_SURFACE_ROUGHNESS_) 
      DefineSkinSurface("WlsSurface", wls_log, _WLS_SURFACE_REFLECTIVITY_, 
			_WLS_SURFACE_ROUGHNESS_);

    // Then add a small "piece of air" next to the rear side and define a diffusive paint;
    {
      G4Box *qq_box = new G4Box("QqPlate", WlsPlateThickness/2, WlsPlateHeight/2, WlsPlateAirAppendixLength/2);
      G4LogicalVolume *qq_log = new G4LogicalVolume(qq_box, mAir, "QqPlate", 0, 0, 0);
      G4PVPlacement *qq_phys = 
	new G4PVPlacement(0, G4ThreeVector(wlsXoffset, wlsYoffset, -(WlsPlateLength+WlsPlateAirAppendixLength)/2), 
			  qq_log, "QqPlate", tower_envelope_log, false, 0);
      
      // NB: physical surface interface seems to have precedence over logical ones, so this 
      // does not disturb the other 5 facets (pretty much exactly what I need);
      G4OpticalSurface* opWlsPaintSurface = 
	CreateLambertianSurface("WlsPaintSurface", _BC620_PAINT_REFLECTIVITY_);
      new G4LogicalBorderSurface("WlsPaintAirSurface", wls_phys, qq_phys, opWlsPaintSurface);
      new G4LogicalBorderSurface("AirPaintWlsSurface", qq_phys, wls_phys, opWlsPaintSurface);
    }
  } 

  // Reflective mylar volume; length and height are the same as WLS plate;
#ifdef _HCAL_USE_MYLAR_REFLECTOR_
  {
    G4Box *mylar_box = new G4Box("Mylar", MetallizedMylarThickness/2, WlsPlateHeight/2, WlsPlateLength/2);
    //G4Box *mylar_box = new G4Box("Mylar", 80*mm/2, 80*mm/2, 3*mm/2);
    G4LogicalVolume *mylar_log = new G4LogicalVolume(mylar_box, mMylar, "Mylar", 0, 0, 0);

    double xOffset =  (TowerEnvelopeWidth - MetallizedMylarThickness)/2 - SmallGap;
    G4PVPlacement *mylar_phys = 
      new G4PVPlacement(0, G4ThreeVector(xOffset, wlsYoffset, 0), mylar_log, "Mylar", tower_envelope_log, false, 0);
    //new G4PVPlacement(0, G4ThreeVector(), mylar_log, "Mylar", tower_envelope_log, false, 0);

    // FIXME: may eventually want to unify with CreateLambertianSurface() call;
    G4OpticalSurface* opMylarSurface = new G4OpticalSurface("MylarSurface");
    opMylarSurface->SetType(dielectric_metal);
    // NB: assume specular reflection, right?;
    opMylarSurface->SetFinish(polished);
    opMylarSurface->SetModel(unified);
      
    G4double reflectivity[_GDIM_];
    for(int iq=0; iq<_GDIM_; iq++) 
      reflectivity[iq] = _HCAL_MYLAR_REFLECTIVITY_;
    G4MaterialPropertiesTable *mylarST = new G4MaterialPropertiesTable();
    mylarST->AddProperty("REFLECTIVITY", gPhotonEnergy, reflectivity, _GDIM_);
    opMylarSurface->SetMaterialPropertiesTable(mylarST);

    new G4LogicalBorderSurface("MylarSurface", tower_envelope_phys, mylar_phys, opMylarSurface);
  }
#endif

  // Install zebra filter between scintillator plates and the WLS;
#ifdef _USE_ZEBRA_FILTER_
  {
    double xOffset = wlsXoffset - WlsPlateThickness/2 - SmallGap - ZebraFilterThickness/2;

    // Loop through all of them and install absorpting volumes; 
    for(unsigned iq=0; iq<_HCAL_MASK_LINE_NUM_; iq++) {
      HcalMaskLine *line = HcalMaskLines + iq;
      
      char name[1024];
      snprintf(name, 1024-1, "ZebraLine%2d", iq);

      G4Box *black_box = new G4Box(name, ZebraFilterThickness/2*mm, line->width/2*mm, line->length/2*mm);
      G4LogicalVolume *black_log = new G4LogicalVolume(black_box, mMylar, name, 0, 0, 0);
      
      double zOffset = (WlsPlateLength - line->length*mm)/2;

      for(unsigned tb=0; tb<2; tb++) {
	double yOffset = wlsYoffset + (tb ? -1.0 : 1.0)*line->y0*mm;

	// Only one central line, clear;
	if (!iq && tb) continue;

	//G4VPhysicalVolume *black_phys = 
	new G4PVPlacement(0, G4ThreeVector(xOffset, yOffset, zOffset), black_log, name, tower_envelope_log, false, tb);
      } //for tb
    } //for iq
  }
#endif

  // Use fake "sensor", directly attached to WLS and matching its size; 
  {
    G4Box *sipm_box = new G4Box("SiPM", SiPMSize/2, WlsPlateHeight/2, SiPMThickness/2);
    G4LogicalVolume *sipm_log = new G4LogicalVolume(sipm_box, mSilicon, "SiPM", 0, 0, 0);
    G4VisAttributes *sipmVisAtt = new G4VisAttributes(G4Colour(0.,1.,1.));
    sipm_log->SetVisAttributes(sipmVisAtt);

    // NB: keep SmallGap here in order to avoid off-time shallow angle travel in WLS;
    double zOffsetSiPM = WlsPlateLength/2 + /*OpticalGlueThickness +*/ SiPMThickness/2 + SmallGap;

    new G4PVPlacement(0, G4ThreeVector(wlsXoffset, wlsYoffset, zOffsetSiPM), sipm_log, 
		      "SiPM", tower_envelope_log, false, 0);

    // SiPM "sensitive layer"; FIXME: do it better later; 
    G4Box *sensor_box = new G4Box("Sensor", SiPMSize/2, WlsPlateHeight/2, SensorThickness/2);
    G4LogicalVolume *sensor_log = new G4LogicalVolume(sensor_box, mQair, "Sensor", 0, 0, 0);
    new G4PVPlacement(0, G4ThreeVector(), sensor_log, "Sensor", sipm_log, false, 0);
  }

#if _TODAY_
  // SiPM volumes; FIXME: for now assume direct WLS->glue->SiPM contact
  // with no reflections, etc; NB: SiPM is an envelope here (sensitive metal layer is inside);  
  G4Box *opt_glue_box = new G4Box("OptGlue", SiPMSize/2, SiPMSize/2, OpticalGlueThickness/2);
  G4LogicalVolume *opt_glue_log = new G4LogicalVolume(opt_glue_box, mRTV3145, "OptGlue", 0, 0, 0);

  G4Box *sipm_box = new G4Box("SiPM", SiPMSize/2, SiPMSize/2, SiPMThickness/2);
  G4LogicalVolume *sipm_log = new G4LogicalVolume(sipm_box, mSilicon, "SiPM", 0, 0, 0);
  G4VisAttributes *sipmVisAtt = new G4VisAttributes(G4Colour(0.,1.,1.));
  sipm_log->SetVisAttributes(sipmVisAtt);
  {
    //double xOffset     = (TowerEnvelopeWidth - WlsPlateThickness)/2 - SmallGap;
    double zOffsetGlue = (WlsPlateLength + OpticalGlueThickness)/2;
    double zOffsetSiPM = WlsPlateLength/2 + OpticalGlueThickness + SiPMThickness/2;

#ifdef _USE_31_SIPM_PER_TOWER_
    for(unsigned iq=0; iq<31; iq++) {
    //for(unsigned iq=15; iq<=15; iq++) {
      // FIXME: should a small WLS plate Y-offset be taken into account here?;
      double yOffset = (iq-15.0)*3.02*mm;
	
      //G4VPhysicalVolume *opt_glue_phys = 
      new G4PVPlacement(0, G4ThreeVector(wlsXoffset, yOffset, zOffsetGlue), opt_glue_log, 
			"OptGlue", tower_envelope_log, false, iq);
      
      //G4VPhysicalVolume *sipm_phys = 
      new G4PVPlacement(0, G4ThreeVector(wlsXoffset, yOffset, zOffsetSiPM), sipm_log, 
			"SiPM", tower_envelope_log, false, iq);
    } //for iq
#else
    // 2x4 SiPMs, located at known Y-offsets;
    for(unsigned tb=0; tb<2; tb++) 
      for(unsigned iq=0; iq<4; iq++) {
	// FIXME: should a small WLS plate Y-offset be taken into account here?;
	double yOffset = (tb ? -1.0 : 1.0)*(iq+1)*10.0*mm;
	
	//G4VPhysicalVolume *opt_glue_phys = 
	new G4PVPlacement(0, G4ThreeVector(wlsXoffset, yOffset, zOffsetGlue), opt_glue_log, 
			  "OptGlue", tower_envelope_log, false, tb*4+iq);
	
	//G4VPhysicalVolume *sipm_phys = 
	new G4PVPlacement(0, G4ThreeVector(wlsXoffset, yOffset, zOffsetSiPM), sipm_log, 
			  "SiPM", tower_envelope_log, false, tb*4+iq);
    } //for tb..iq
#endif
  }

  // SiPM sensitive layer; FIXME: do it better later; for now assume 
  G4Box *sensor_box = new G4Box("Sensor", SiPMSize/2, SiPMSize/2, SensorThickness/2);
  G4LogicalVolume *sensor_log = new G4LogicalVolume(sensor_box, mQair, "Sensor", 0, 0, 0);
  new G4PVPlacement(0, G4ThreeVector(), sensor_log, "Sensor", sipm_log, false, 0);
#endif

  // Tower steel spacers; assume I can pack it without gaps (optical part will proceed in 
  // the light tight air envelope anyway); so it's only the steel material which matters; 
  if (mGeometry->mUseSteelSpacers) {
    G4Box *spacer_box = new G4Box("Spacer", TowerWidth/2, mGeometry->mSteelSpacerThickness/2, TowerLength/2);
    G4LogicalVolume *spacer_log = new G4LogicalVolume(spacer_box, mSteel, "Spacer", 0, 0, 0);
    {
      double yOffset = TowerHeight/2 - mGeometry->mSteelSpacerThickness/2;
      
      new G4PVPlacement(0, G4ThreeVector(0, yOffset, 0), 
			spacer_log, "Spacer", tower_log, false, 0);
    }
  } //if
#endif

  return expHall_phys;
} // HCalDetectorConstruction::DefineHCalVolumes()

// -------------------------------------------------------------------------------------

G4VPhysicalVolume* HCalDetectorConstruction::Construct()
{
  // Global photon energy range; [360..600] nm;
  for(int iq=0; iq<_GDIM_; iq++)
    gPhotonEnergy[iq] = 1240.0/(600.0 - iq*10.)*eV;

  // Chemical elements;
  DefineElements();

  // Materials;
  DefineMaterials();

  // HCal volumes;
  return DefineHCalVolumes();
} // HCalDetectorConstruction::Construct()

// -------------------------------------------------------------------------------------

