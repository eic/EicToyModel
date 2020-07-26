
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//#pragma link C++ namespace qeic;

#pragma link C++ class EicPOD+;
#pragma link C++ class EicDetName+;

#pragma link C++ class EicNamePatternHub<unsigned>+;
#pragma link C++ class EicNamePatternHub<double>+;
//#pragma link C++ class EicNamePatternHub<SteppingType>+;
//#pragma link C++ class std::pair<TString,SteppingType>+;
#pragma link C++ class std::pair<TString,unsigned>+;
#pragma link C++ class EicNamePatternHub<Color_t>+;
#pragma link C++ class EicNamePatternHub<Char_t>+;
#pragma link C++ class std::pair<TString,Color_t>+;
#pragma link C++ class std::pair<TString,Char_t>+;

#pragma link C++ class std::pair<TString,Color_t>+;
#pragma link C++ class std::pair<TString,Char_t>+;
#pragma link C++ class EicBitMask<UGeantIndex_t>+;
#pragma link C++ class EicBitMask<ULogicalIndex_t>+;
#pragma link C++ class GeantVolumeLevel+;
#pragma link C++ class EicGeoMap+;
#pragma link C++ class SourceFile+;
#pragma link C++ class LogicalVolumeGroupProjection+;
#pragma link C++ class LogicalVolumeGroup+;
#pragma link C++ class EicGeoParData+;
#pragma link C++ class LogicalVolumeLookupTableEntry+;
#pragma link C++ class std::pair<ULong64_t,ULong64_t>+;
#pragma link C++ class std::pair<TString, Int_t>+;

#pragma link C++ class GemModule+;
#pragma link C++ class GemWheel+;
#pragma link C++ class GemGeoParData+;
#pragma link C++ class MuMegasLayer+;
#pragma link C++ class MuMegasBarrel+;
#pragma link C++ class MuMegasGeoParData+;
#pragma link C++ class MapsGeoParData+;
#pragma link C++ class VstGeoParData+;
#pragma link C++ class FstGeoParData+;
#pragma link C++ class FstDisc+;
#pragma link C++ class VstBarrelLayer+;
#pragma link C++ class MapsStave+;
#pragma link C++ class MapsMimosaAssembly+;

#pragma link C++ namespace etm;

#pragma link C++ class EtmLine2D+;
#pragma link C++ class EtmEtaBoundary+;
#pragma link C++ class EtmBoundaryModificationRequest+;
#pragma link C++ class EtmPolygonGroup+;
#pragma link C++ class EtmPaletteEntry+;
#pragma link C++ class EtmPalette+;
#pragma link C++ class EtmVacuumChamber+;
#pragma link C++ class EtmAzimuthalScan+;
#pragma link C++ class EtmDetector+;
#pragma link C++ class EtmDetectorStack+;

#pragma link C++ class EicToyModel+;

//#pragma link C++ class XString+;

#pragma link C++ class vc2020_03_20+;

// ROOT 6.18 requires this; was not needed in 6.14;
#pragma link C++ class std::pair<bool,bool>+;

#endif
