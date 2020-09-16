//
// AYK (ayk@bnl.gov), 2014/08/07
//
//  FIXME: materials are hardcoded indeed; but I know, they are well defined ...;
//

#include <iostream>
using namespace std;

#include <TMath.h>
#include <TGeoTrd1.h>
#include <TGeoTube.h>
#include <TVector2.h>

#include <EtmOrphans.h>
#include <VstGeoParData.h>

// ---------------------------------------------------------------------------------------

VstGeoParData::VstGeoParData(int version, int subVersion): 
  MapsGeoParData(_VST_DETECTOR_NAME_, version, subVersion), 
  mMountingRingRadialOffset      ( 3.00 * etm::mm)
{
} // VstGeoParData::VstGeoParData()

// ---------------------------------------------------------------------------------------

int VstGeoParData::ConstructGeometry(bool root, bool gdml, bool check)
{
  // FIXME: may want to put these into MapsGeoParData?!;
  unsigned staveGlobalCounter = 0, chipGlobalCounter = 0;

  for(unsigned bl=0; bl<GetNumberOfLayers(); bl++) {
    const VstBarrelLayer *blayer = GetBarrelLayer(bl);
    MapsMimosaAssembly *mcell = blayer->mChipAssembly;

    staveGlobalCounter += blayer->mStaveNum;
    chipGlobalCounter  += blayer->mStaveNum*blayer->mMimosaChipNum;

    AddLogicalVolumeGroup(blayer->mStaveNum, 0, blayer->mMimosaChipNum);

    char mountingRingName[128];

    snprintf(mountingRingName,     128-1, "VstMountingRing%02d",        bl);
 
    MapsStave *stave = ConstructStave(blayer->mMimosaChipNum, bl, mcell);

    // Yes, carelessly create one map per layer;
    EicGeoMap *fgmap = CreateNewMap();
    fgmap->AddGeantVolumeLevel(mMimosaCoreName,   0);
    fgmap->AddGeantVolumeLevel(mMimosaShellName,  0);
    fgmap->AddGeantVolumeLevel(mCellAssemblyName, blayer->mMimosaChipNum);
    fgmap->AddGeantVolumeLevel(stave->GetName(),  blayer->mStaveNum);

    fgmap->SetSingleSensorContainerVolume(mMimosaCoreName);

    // Construct mapping table; no tricks like linear cell numbering and circular 
    // quadrant arrangement in say FEMC calorimeter construction -> may just use a separate loop;
    for(unsigned st=0; st<blayer->mStaveNum; st++) 
      for(unsigned nn=0; nn<blayer->mMimosaChipNum; nn++) {
	UInt_t geant[4] = {0, 0, nn, st}, logical[3] = {st, 0, nn};

	if (SetMappingTableEntry(fgmap, geant, bl, logical)) {
	  cout << "Failed to set mapping table entry!" << endl;
	  exit(0);
	} //if
      } //for st..nn

    {
      // ALICE TDR gives silicon chip center intallation radius (p.8); want to reproduce the 
      // geometry (and slope in particular) in order to cross-check material budget;
      double alfa = blayer->mStaveSlope*TMath::Pi()/180.0;
      // Use sine theorem or such;
      double beta = asin(fabs(mMimosaOffset)*sin(alfa)/blayer->mRadius);
      double gamma = TMath::Pi() - alfa - beta;
      double staveCenterRadius = blayer->mRadius*sin(gamma)/sin(alfa);

      // Figure out thickness of the overall air container volume; 
      double H = GetAssemblyContainerWidth(mcell), W = mcell->GetAssemblyHeight();
      TVector2 A(-W/2, -H/2), B(W/2, -H/2), C(0.0, H/2), *arr[3] = {&A, &B, &C};
      double rmin = 0.0, rmax = 0.0;
      for(unsigned iq=0; iq<3; iq++) {
	TVector2 pt = arr[iq]->Rotate(alfa) + TVector2(0.0, staveCenterRadius);
	double r = pt.Mod();

	if (!iq || r < rmin) rmin = r;
	if (!iq || r > rmax) rmax = r;
      } //for iq
      //printf("%f %f %f\n", rmin, blayer->mRadius, rmax);
      
      // Define air container volume and place it into the top volume;
      char barrelContainerVolumeName[128];
      snprintf(barrelContainerVolumeName, 128-1, "%sBarrelContainerVolume%02d", "Vst", bl);
      
      TGeoTube *bcontainer = new TGeoTube(barrelContainerVolumeName,
					  rmin,
					  rmax,
					  stave->GetLength()/2);
      TGeoVolume *vbcontainer = new TGeoVolume(barrelContainerVolumeName, bcontainer, GetMedium(_AIR_));
      
      GetTopVolume()->AddNode(vbcontainer, 0, 0);//barrel->mTransformation);
      
      // Place staves into master volume; FIXME: no extral hierarchy here?; well, otherwise 
      // would have to precisely calculate barrel TUBE volume inner/outer radius; 
      for(unsigned st=0; st<blayer->mStaveNum; st++) {
	TGeoRotation *rw = new TGeoRotation();
	  
	double degAngle = st*360.0/blayer->mStaveNum + blayer->mAsimuthalOffset;
	double radAngle = degAngle*TMath::Pi()/180.0;
	double fullAngle = degAngle + blayer->mStaveSlope;

	rw->SetAngles(90.0, 0.0 - fullAngle, 180.0,  0.0, 90.0, 90.0 - fullAngle);

	  vbcontainer->AddNode(stave->GetVolume(), st, 
			       new TGeoCombiTrans(staveCenterRadius*sin(radAngle), 
						  staveCenterRadius*cos(radAngle), 0.0, rw));
      } //for st
      
      // Construct a mounting ring; let it be there for all geometry types;
      if (mWithMountingRings) {
	printf("%f %f %f\n", (staveCenterRadius + mMountingRingRadialOffset - mMountingRingRadialThickness/2),
				       (staveCenterRadius + mMountingRingRadialOffset + mMountingRingRadialThickness/2),
				       mMountingRingBeamLineThickness/2);
	TGeoTube *mring = new TGeoTube(mountingRingName,
				       (staveCenterRadius + mMountingRingRadialOffset - mMountingRingRadialThickness/2),
				       (staveCenterRadius + mMountingRingRadialOffset + mMountingRingRadialThickness/2),
				       mMountingRingBeamLineThickness/2);
	TGeoVolume *vmring = new TGeoVolume(mountingRingName, mring, GetMedium(mCarbonFiberMaterial));
	
	// Place two rings; FIXME: it looks like they do not belong to the container volume?;
	for(unsigned fb=0; fb<2; fb++) {
	  double zOffset = (fb ? -1. : 1.)*(stave->GetLength()/2 + mMountingRingBeamLineThickness/2);
	  
	  GetTopVolume()->AddNode(vmring, fb, new TGeoCombiTrans(0.0, 0.0, zOffset, 0));
	} //for fb
      } //if
    }
  } //for bl
   
  //printf("%5d chip(s) and %5d stave(s) total\n", chipGlobalCounter, staveGlobalCounter);

  // Color palette; FIXME: what about transparency?;
  GetColorTable()->AddPatternMatch("WaterPipe",      kYellow);
  GetColorTable()->AddPatternMatch("Water",          kBlue);
  GetColorTable()->AddPatternMatch("StaveBracket",   kOrange);
  GetColorTable()->AddPatternMatch("Beam",           kBlack);
  GetColorTable()->AddPatternMatch("ColdPlate",      kYellow);
  GetColorTable()->AddPatternMatch("MimosaCore",     kYellow);
  GetColorTable()->AddPatternMatch("CellFlexLayer",  kGreen+2);
  GetColorTable()->AddPatternMatch("AluStrips",      kGray);
  GetColorTable()->AddPatternMatch("MountingRing",   kMagenta+1);

  // Place this stuff as a whole into the top volume and write out;
  FinalizeOutput(root, gdml, check);

  return 0;
} // VstGeoParData::ConstructGeometry()

// ---------------------------------------------------------------------------------------

ClassImp(VstGeoParData)
ClassImp(VstBarrelLayer)
