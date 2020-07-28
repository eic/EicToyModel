//
// AYK (ayk@bnl.gov), 2014/08/11
//
//  MAPS Mimosa 34 container description;
//

#include <EtmOrphans.h>
#include <MapsMimosaAssembly.h>

// ---------------------------------------------------------------------------------------

MapsMimosaAssembly::MapsMimosaAssembly( void ):
  // Air container volume parameters sufficient to pack all the stuff;
  mAssemblyBaseWidth           (   17.500 * etm::mm),
  mAssemblySideSlope           (   30.000 * etm::mm),
  mChipToChipGap               (    0.100 * etm::mm),

  // Space structure;
  mApexEnforcementBeamDiameter (    0.400 * etm::mm),
  mEnforcementStripWidth       (    0.500 * etm::mm),
  mEnforcementStripThickness   (    0.200 * etm::mm),
  mBaseEnforcementBeamWidth    (    1.200 * etm::mm),
  mSideWallThickness           (    0.050 * etm::mm),

  // Basic Mimosa 34 chip parameters; pixel size does not matter here (will play 
  // a role during digitization only);
  mChipLength                  (   30.000 * etm::mm),
  mChipWidth                   (   15.000 * etm::mm),
  // Well, ALICE rad.length scan plot indicates, that it is equal to the chip width;
  // NB: do NOT try to set this value to be larger than mChipWidth (unless modify
  // assembly and stave width calculation):
  mAssemblyDeadMaterialWidth   (   15.000 * etm::mm),
  mChipThickness               (    0.050 * etm::mm),
  mChipActiveZoneThickness     (    0.018 * etm::mm),
  mChipDeadAreaWidth           (    2.000 * etm::mm),

  // Layers at the base of the assembly; kapton and effective alu thinckness;
  mFlexCableKaptonThickness    (    0.100 * etm::mm),
  // Based on the ALICE rad.length scan one can conclude, that 50um is too much;
  // I guess these strips occupy only a fraction of the assembly width;
  mFlexCableAluThickness       (    0.020 * etm::mm),
  // Assume just 'Carbon fiber + paper' on p.55 of ALICE ITS TDR) -> 30+70um;
  mColdPlateThickness          (    0.100 * etm::mm),

  // Water pipes; assume 2 parallel pipes; 25um thick walls;
  mWaterPipeInnerDiameter      (    1.024 * etm::mm),
  mWaterPipeWallThickness      (    0.025 * etm::mm)
{
} // MapsMimosaAssembly::MapsMimosaAssembly()

// ---------------------------------------------------------------------------------------

ClassImp(MapsMimosaAssembly)
