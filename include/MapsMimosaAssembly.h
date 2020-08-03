//
// AYK (ayk@bnl.gov), 2014/08/11
//
//  MAPS a la ALICE ITS2 Alpide chip "container" description;
//

#include <TMath.h>

#include <EicPOD.h>
#include <EicGeoParData.h>

#ifndef _MAPS_MIMOSA_ASSEMBLY_
#define _MAPS_MIMOSA_ASSEMBLY_

class MapsMimosaAssembly: public EicPOD {
 public:
  MapsMimosaAssembly();
  MapsMimosaAssembly(MapsMimosaAssembly *source) { *this = *source; };
  ~MapsMimosaAssembly() {};
  
  //
  //  Data container -> no private section;
  //

  // Air container volume parameters sufficient to pack all the stuff;
  Double_t mAssemblyBaseWidth;          // air container TRD1 (with 0 width apex) volume base width
  Double_t mAssemblySideSlope;          // angle at the base of triangular container profile; [degree]
  Double_t mChipToChipGap;              // gap between neighboring chips on a stave

  // Space structure;
  Double_t mApexEnforcementBeamDiameter;// diameter of the thin support tube at the assembly apex
  Double_t mBaseEnforcementBeamWidth;   // width of support triangles at the assemble edges
  Double_t mEnforcementStripWidth;      // width of side wall enforcement strip
  Double_t mEnforcementStripThickness;  // thickness of side wall enforcement strip
  Double_t mSideWallThickness;          // thickness of container side walls

  // Basic chip parameters; pixel size does not matter here (will play 
  // a role during digitization only);
  Double_t mChipLength;                 // chip length along the stave (30mm)
  Double_t mChipWidth;                  // chip width (15mm)
  Double_t mChipThickness;              // silicon thickness (50um)
  Double_t mChipActiveZoneThickness;    // active layer thickness (assume 20um in the middle)
  Double_t mChipDeadAreaWidth;          // dead area along the long side (2mm or so)

  Double_t mAssemblyDeadMaterialWidth;  // cold head, etc can be wider than the chip

  // Layers at the base of the assembly;
  Double_t mFlexCableKaptonThickness;   // flex cable substrate thickness
  Double_t mFlexCableAluThickness;      // flex cable effective aluminum bus strips thickness
  Double_t mColdPlateThickness;         // cold plate thickness

  // Water pipes; assume 2 parallel pipes;
  Double_t mWaterPipeInnerDiameter;     // water pipe inner diameter
  Double_t mWaterPipeWallThickness;     // water pipe wall thickness

  double GetAssemblyLength() const { return mChipLength + mChipToChipGap; };

  double GetAssemblyHeight() const { return (mAssemblyBaseWidth/2)*
      tan(mAssemblySideSlope*TMath::Pi()/180.);
  };

  ClassDef(MapsMimosaAssembly,14);
};

#endif
