//
// AYK (ayk@bnl.gov), 2013/06/13
//
//  EIC geometry mapping classes;
//

#include <fstream>

#include <stdlib.h>
#include <assert.h>
#include <iostream>
// This is needed for Mac OS only;
#ifdef __APPLE__
#include <unistd.h>
#include <libgen.h>
#endif

#include <TROOT.h>
#include <TClass.h>
#include <TColor.h>
#include <TRealData.h>
#include <TDataMember.h>

#ifdef _VGM_
#include "BaseVGM/volumes/VFactory.h"
#include "VGM/volumes/IPlacement.h"
#include "Geant4GM/volumes/Factory.h"
#include "RootGM/volumes/Factory.h"
#endif

#include <EtmOrphans.h>
#include <EicGeoMedia.h>
#include <EicGeoParData.h>

#ifdef _ETM2GEANT_
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#endif

EicGeoMedia *EicGeoParData::mEicMedia = 0;

// =======================================================================================
 
//
// Constructor & Co
//

void EicGeoParData::ResetVars() 
{ 
  mGeometryType = Undefined;
  mTestGeometryFlag = false;
  mMaxVolumeLevelNum = 0;

  {
    char user[128], host[128];
    
    getlogin_r(user, 128-1);
    gethostname(host, 128-1);
    mAuthor = TString(user) + "@" + host;
  }
  
  mTimeStamp = TTimeStamp();
  
  mTopVolumeTransformation = 0;
  
  mDetName = 0; mRootGeoManager = mSavedGeoManager = 0; mWrapperVolume = mTopVolume = 0; 
  //@@@mGeoLoad = 0; mGeoFace = 0; mGeobuild = 0;
  //@@@mEicMedia = 0;  
  mSavedGeoIdentity = 0;

  mColorRequests = 0; mTransparencyRequests = 0;

  mTransparency = 0;

  mGeometryCheckPrecision = 1.0 *etm::um;//.0;
} // EicGeoParData::ResetVars() 

// ---------------------------------------------------------------------------------------

EicGeoParData::EicGeoParData(const char *detName, int version, int subVersion):
  mVersion(version), mSubVersion(subVersion)
{
  ResetVars();

  // Create detector name class;
  mDetName   = new EicDetName(detName);

  // Well, default constructor of some detectors (like VST) will call VstGeoParData()
  // as EicGeoParData("VST"); so can not just check on detName=0 here in order to see 
  // whether I reached this place from EicDetector::ConstructGeometry() when ROOT
  // streamer imported EicGeoParData block, rather than from geometry-producing script 
  // like vst.C; do not want to change default calls for those constructors; so 
  // conventionally check here on FairRun::Instance() and do not mess up with the running 
  // FairRun if it was instantiated (meaning most likely from under the running simulation);
#if _TODAY_
  if (FairRun::Instance()) return;

  // No ExpnandedFileName() tricks here, please;
  TString mediaFileName  = TString(getenv("VMCWORKDIR")) + "/geometry/media.geo";

  mGeoLoad   = FairGeoLoader::Instance() ? FairGeoLoader::Instance() : 
    new FairGeoLoader("TGeo", "FairGeoLoader");
  mGeoFace   = mGeoLoad->getGeoInterface();

  mGeoFace->setMediaFile(mediaFileName);
  mGeoFace->readMedia();

  mFairMedia = mGeoFace->getMedia();
  mGeobuild  = mGeoLoad->getGeoBuilder();
#endif

  // FIXME: for EicRoot geometries, which require construction elements in the 
  // .C scripts rather than in a library call, will have to switch/restore by hand;
  //SwitchGeoManager();
#if _MOVED_
  // Initialize Geo manager and create basic geometry volume structure; 
  mRootGeoManager = new TGeoManager();

  // It looks like 2-level layout (with an extra MASTER volume) is required by FairRoot;
  mWrapperVolume = new TGeoVolumeAssembly(mDetName->Name() + "GeantGeoWrapper");
  mTopVolume     = new TGeoVolumeAssembly(mDetName->NAME());

  mRootGeoManager->SetTopVolume(mWrapperVolume);
#endif
} // EicGeoParData::EicGeoParData()

// =======================================================================================

int EicGeoParData::ImportMediaFile(const char *fname)
{
  std::fstream fin(fname, std::fstream::in);
  if (!fin.is_open()) {
    printf("Media file '%s' does not exist!\n", fname);
    return -1;
  } //if

  mEicMedia = new EicGeoMedia();
  
  mEicMedia->read(fin);
  //mEicMedia->print();

  return 0;
} // EicGeoParData::ImportMediaFile()

// =======================================================================================

void EicGeoParData::SwitchGeoManager( void )
{
  // Save the original pointers; this is dumb of course (and assumes that a switch back 
  // happens in the same call (which I kind of control myself); indeed ROOT is an awkward 
  // code in terms of using global variables, etc;
  mSavedGeoManager  = gGeoManager;
  mSavedGeoIdentity = gGeoIdentity;

  // Initialize Geo manager and create basic geometry volume structure; 
  mRootGeoManager = new TGeoManager();

  // It looks like 2-level layout (with an extra MASTER volume) is required by FairRoot;
  mWrapperVolume = new TGeoVolumeAssembly(mDetName->Name() + "GeantGeoWrapper");
  mTopVolume     = new TGeoVolumeAssembly(mDetName->NAME());

  mRootGeoManager->SetTopVolume(mWrapperVolume);
} // EicGeoParData::SwitchGeoManager()

// ---------------------------------------------------------------------------------------

void EicGeoParData::RestoreGeoManager( void )
{
  // This will remove the pointer from the list of geometries and the list of browsables, 
  // see TGeoManager::~TGeoManager() code;
  delete mRootGeoManager;

  gGeoManager  = mSavedGeoManager;
  gGeoIdentity = mSavedGeoIdentity;
} // EicGeoParData::RestoreGeoManager()

// ---------------------------------------------------------------------------------------

//void EicGeoParData::PlaceG4Volume(G4VPhysicalVolume *mother, bool check, 
void EicGeoParData::PlaceG4Volume(G4LogicalVolume *mother, bool check, 
				  //G4RotationMatrix *pRot, G4ThreeVector *tlate)
				  void *pRot, void *tlate)
{
  // Switch() & Restore() calls are generic;
  SwitchGeoManager();

  // This one is virtual, detector-specific (populates gGeoManager); 
  // perform TGeo geometry check (optional);
  ConstructGeometry(false, false, check);

#if defined(_ETM2GEANT_) && defined(_VGM_)
  // Create std::set of the sensitive volume names;
  mSensitiveVolumeNames.clear(); 
  mG4Volumes.clear();
  mG4SensitiveVolumes.clear();
  for(int iq=0; iq<GetMapNum(); iq++) {
    const EicGeoMap *fmap = GetMapPtrViaMapID(iq);

    //printf("@@@ %s\n", fmap->GetInnermostVolumeName()->Data());
    //mSensitiveVolumeNames.insert(*fmap->GetInnermostVolumeName());
    assert(mSensitiveVolumeNames.find(*fmap->GetInnermostVolumeName()) ==
	   mSensitiveVolumeNames.end());
    mSensitiveVolumeNames[*fmap->GetInnermostVolumeName()] = iq;
  } //if..for iq

  {
    //auto mtable = G4Material::GetMaterialTable();
    //printf("mtable: %d\n", mtable->size());
  }

  // This is again generic; assumes FairRoor two-layer assembly on top of everything, 
  // and then container volumes with the actual stuff inside it;
  if (gGeoManager && gGeoManager->GetTopNode() && gGeoManager->GetTopNode()->GetVolume()->GetNode(0)) {
    auto assembly = gGeoManager->GetTopNode()->GetVolume()->GetNode(0)->GetVolume();
    
    for(int iq=0; iq<assembly->GetNdaughters(); iq++) {
      // FIXME: does one really need to create this pair of factories every time new?;
      RootGM::Factory rtFactory;
      Geant4GM::Factory g4Factory;

      auto node = assembly->GetNode(iq);
      //printf("@@@ -> %s\n", node->GetName());

      rtFactory.Import(node);
      rtFactory.Export(&g4Factory);

      // FIXME: this is a hack (assumes [mm], which seems to be correct; but also will not work with rotations); 
      auto trans = rtFactory.Top()->Transformation();
      //for(auto value: trans)
      //printf("%f\n", value);
      G4ThreeVector offset(trans[0], trans[1], trans[2]);
      if (tlate) offset += *(G4ThreeVector*)tlate;
      
      // FIXME: this is not good; but 'class G4ThreeVector' followed by '#include "G4ThreeVector.hh"'
      // fails because of the CLHEP typedef conflict;
      //printf("@@@ %f\n", g4Factory.World()->GetTranslation()[2]);
      //auto pvol = new G4PVPlacement(g4Factory.World()->GetRotation(), g4Factory.World()->GetTranslation(), 
      auto pvol = new G4PVPlacement((G4RotationMatrix*)pRot, offset,
				    g4Factory.World()->GetLogicalVolume(), 
				    node->GetName(), mother/*->GetLogicalVolume()*/, false, 0);
      AssignG4Colors(pvol);
    } //for iq
  }

  {
    //auto mtable = G4Material::GetMaterialTable();
    //printf("mtable: %d\n", mtable->size());

    //mtable->clear();
  }

  //printf("%d %d\n", GetG4Volumes().size(), GetG4SensitiveVolumes().size());
#endif
  
  RestoreGeoManager();
} // EicGeoParData::PlaceG4Volume()

// ---------------------------------------------------------------------------------------

void EicGeoParData::AssignG4Colors(G4VPhysicalVolume *pvol)
{
#ifdef _ETM2GEANT_
  auto lvol = pvol->GetLogicalVolume();

  // Deal with the volume vectors here as well; FIXME: change the method name;
  mG4Volumes.push_back(pvol);
  //if (mSensitiveVolumeNames.find(lvol->GetName()) != mSensitiveVolumeNames.end())
  //mG4SensitiveVolumes.push_back(pvol);
  if (mSensitiveVolumeNames.find(lvol->GetName()) != mSensitiveVolumeNames.end())
    mG4SensitiveVolumes[pvol] = mSensitiveVolumeNames[lvol->GetName()];// + 1;
  
  // Follow the logic of EicGeoParData::FinalizeOutput();
  {
    // FIXME: memory leak?;
    G4VisAttributes* visAtt = new G4VisAttributes();
    // Get volume color; if not defined, volume should be invisible;
    const std::pair<TString, Color_t> *cpattern = GetColorTable()->AnyMatch(lvol->GetName());
    
    if (cpattern) {
      auto rcolor = gROOT->GetColor(cpattern->second);

      // Get volume transparency value (8-bit character 0..100 in the ROOT world);
      const std::pair<TString, Char_t> *tpattern = GetTransparencyTable()->AnyMatch(lvol->GetName());
      double transparency = tpattern ? (100-tpattern->second)/100. : 1.0;
      visAtt->SetColor(rcolor->GetRed(), rcolor->GetGreen(), rcolor->GetBlue(), transparency);
      visAtt->SetVisibility(true);
      visAtt->SetForceWireframe(false);
      visAtt->SetForceSolid(true);
    }
    else
      visAtt->SetVisibility(false);
    
    lvol->SetVisAttributes(visAtt);
  }

  for(int iq=0; iq<lvol->GetNoDaughters(); iq++) {
    auto daughter = lvol->GetDaughter(iq);
      
    //printf("--@@@ %s\n", daughter->GetName().data());

    // Call recursively;
    AssignG4Colors(daughter);
  } //for iq  
#endif
} // EicGeoParData::AssignG4Colors()

// ---------------------------------------------------------------------------------------

//
// Mapping file creation part
//

EicGeoMap *EicGeoParData::CreateNewMap() 
{
  EicGeoMap *map = new EicGeoMap();
  mGeantVolumeMaps.push_back(map);
  return map;
} // EicGeoParData::CreateNewMap()

// ---------------------------------------------------------------------------------------

int EicGeoParData::AddLogicalVolumeGroup(unsigned dimX, unsigned dimY, unsigned dimZ)
{
  if (mLogicalVolumeGroups.size() == _LOGICAL_GROUP_NUM_MAX_) {
    printf("Logical table size is full (%d entries)!\n", (unsigned)_LOGICAL_GROUP_NUM_MAX_);
    return -1;
  } //if

  // Arrange a unified access;
  unsigned dim[_LOGICAL_VOLUME_GROUP_COORD_NUM_] = {dimX, dimY, dimZ}, max = 0;

  // NB: there can be 0 dimensions in the middle -> so really want to see what is
  // the max one before populating the vector;
  for(unsigned iq=0; iq<_LOGICAL_VOLUME_GROUP_COORD_NUM_; iq++)
    if (dim[iq]) max = iq;

  LogicalVolumeGroup *group = new LogicalVolumeGroup();

  unsigned accu = 0;
  for(unsigned iq=0; iq<=max; iq++) 
    if (dim[iq]) {
      LogicalVolumeGroupProjection *coord = new LogicalVolumeGroupProjection(dim[iq]);

      EicBitMask<ULogicalIndex_t> *mask = coord->mBitMask;

      mask->SetShift(accu);
      accu += mask->GetBitNum();

      group->mProjections.push_back(coord);
    }
    else
      group->mProjections.push_back(0);
  
  // FIXME: do a better check later;
  assert(accu <= _LOGICAL_XYZ_BIT_NUM_);

  mLogicalVolumeGroups.push_back(group);

  //return 0;
  return mLogicalVolumeGroups.size() - 1;
} // EicGeoParData::AddLogicalVolumeGroup()

// ---------------------------------------------------------------------------------------

int EicGeoParData::SetCircularCore(unsigned group, unsigned what) 
{
  // FIXME: these few lines should be shaped up as a routine!; 
  if (group >= mLogicalVolumeGroups.size()) return -1;
  
  const std::vector<LogicalVolumeGroupProjection*> &coords = 
    mLogicalVolumeGroups[group]->mProjections;
  
  if (what >= coords.size() || !coords[what]) return -1;
  
  coords[what]->mCircular = true;
  
  return 0;
} // EicGeoParData::SetCircularCore()

// ---------------------------------------------------------------------------------------

Int_t EicGeoParData::SetMappingTableEntry(EicGeoMap *map, const unsigned geo[], 
					  unsigned group, unsigned logical[]) 
{
  if (!map || !geo || !logical) {
    printf("-E- EicGeoParData::SetMappingTableEntry(): zero pointer(s) used as arguments!\n"); 
    return -1;
  } //if
    
  if (group >= _LOGICAL_GROUP_NUM_MAX_)  {
    printf("-E- EicGeoParData::SetMappingTableEntry(): group index '%d' exceeds '%d'!\n", 
	   group, (unsigned)_LOGICAL_GROUP_NUM_MAX_); 
    return -1;
  } //if

  // Pack logical[] array into a single ULogicalIndex_t value;
  ULogicalIndex_t value = ULogicalIndex_t(group) << _LOGICAL_XYZ_BIT_NUM_;

  if (group >= mLogicalVolumeGroups.size())  {
    printf("-E- EicGeoParData::SetMappingTableEntry(): group index '%d' exceeds "
	   "configured array size (%d)!\n", group, (unsigned)mLogicalVolumeGroups.size()); 
    return -1;
  } //if
  const std::vector<LogicalVolumeGroupProjection*> &coords = 
    mLogicalVolumeGroups[group]->mProjections;
  
  for(unsigned iq=0; iq<coords.size(); iq++) {
    // Zero logical[] value will not contribute anyway;
    if (!logical[iq]) continue;
    
    LogicalVolumeGroupProjection *coord = coords[iq];
    
    // NB: this check is sufficient to guarantee, that overflow in "value"
    // can not happen (because overall mask/shift consistency was checked 
    // during mProjectionDescriptors creation);
    if (!coord) {
      printf("-E- EicGeoParData::SetMappingTableEntry(): coordinate '%d' was not configured!\n", iq);
      return -1;
    } //if
    if (logical[iq] >= coord->mMaxEntryNum) {
      printf("-E- EicGeoParData::SetMappingTableEntry(): logical index #%d (%d) exceeds "
	     "max configured entry num (%d)!\n", iq, logical[iq], (unsigned)coord->mMaxEntryNum); 
      return -1;
    } //if
    
    // This is guaranteed no be non-zero by mProjectionDescriptors initialization;
    EicBitMask<ULogicalIndex_t> *mask = coord->mBitMask;

    value |= (logical[iq] & mask->GetBitMask()) << mask->GetShift();
  } //for iq
  
  return map->SetMappingTableEntry(geo, value);
} // EicGeoParData::SetMappingTableEntry()

// ---------------------------------------------------------------------------------------

//#include <EicMagneticFieldMap.h>

SourceFile::SourceFile(const char *fileName): mFileSize(0), mFileContent(0), mOkFlag(false)
{
  // This will also be the default constructor case;
  if (!fileName) return;

  FILE *fin = fopen(fileName, "r");
  if (!fin) {
    printf("-E- SourceFile::SourceFile(): file '%s' does not exist!\n", fileName);
    return;
  } //if
  
  // NB: strip directory name;
  {
    // FIXME: unify with EicMagneticFieldMap::BasenameWrapper() at some point;
    char buffer[1024];
    snprintf(buffer, 1024-1, "%s", fileName);

    mFileName = TString(basename(buffer));
  }

  // FIXME: there is perhaps a better way to do this?; then change EicStlFile.cxx as well;
  fseek(fin, 0, SEEK_END);
  mFileSize = ftell(fin);
  
  mFileContent = new UChar_t[mFileSize];

  rewind(fin);
  if (fread(mFileContent, 1, mFileSize, fin) == mFileSize) 
    mOkFlag = true;
  else
    delete [] mFileContent;

  fclose(fin);
} // SourceFile::SourceFile()

// ---------------------------------------------------------------------------------------

void SourceFile::Print()
{
  if (!IsOk()) return;

  printf("\n  Attached file name: %s\n", mFileName.Data());
  printf("  --------------------");
  for(unsigned iq=0; iq<mFileName.Length(); iq++) 
    printf("-");

  // Do not overcomplicate things -> just dump to output assuming it was a clean ASCII file;
  printf("\n\n%s\n", mFileContent);
} // SourceFile::Print()

// ---------------------------------------------------------------------------------------

void EicGeoParData::PrintAttachedSourceFile(const char *fileName)
{
  for(unsigned fl=0; fl<mSourceFiles.size(); fl++) {
    SourceFile *file = mSourceFiles[fl];

    if (file->GetFileName().EqualTo(fileName)) {
      file->Print();
      return;
    } //if
  } //for fl

  printf("\n  No file '%s' was attached!\n", fileName);
} // EicGeoParData::PrintAttachedSourceFile()

// ---------------------------------------------------------------------------------------

int EicGeoParData::AttachSourceFile(const char *fileName)
{
  SourceFile *file = new SourceFile(fileName);

  if (file->IsOk()) {
    mSourceFiles.push_back(file);

    return 0;
  }
  else {
    //delete file;
    return -1;
  } //if
} // EicGeoParData::AttachSourceFile()

// ---------------------------------------------------------------------------------------

const TGeoMedium *EicGeoParData::GetMedium(const char *medium) 
{
  // FIXME: allow for user-created media;
  if (!mRootGeoManager || !mEicMedia) return 0;

  if (!mMediaCache.count(medium)) {
    EicGeoMedium *fmedium = mEicMedia->getMedium(medium);
    mEicMedia->createMedium(fmedium);

    mMediaCache.insert(medium);
  } //if

  return mRootGeoManager->GetMedium(medium);
} // EicGeoParData::GetMedium()

// ---------------------------------------------------------------------------------------

TString EicGeoParData::GetGeometryFileName(bool root) const
{
  const char *suffix = root ? ".root" : ".gdml";
  // If file is given, use it;
  if (!mFileName.IsNull()) return mFileName;

  // If file name format is given and version is defined, use them;
  if (!mFileNameFormat.IsNull()/* && GetVersion() >= 0*/) {
    char fileName[128];
    
    if (GetVersion() >= 0)
      snprintf(fileName, 128-1, mFileNameFormat.Data(), GetVersion(), GetSubVersion());
    else
      // At least Clang prefers it this way;
      snprintf(fileName, 128-1, "%s", mFileNameFormat.Data());
      
    return TString(fileName);
  } //if

  // Detector name is not known -> can hardly do anything -> return empty string;
  if (!mDetName) return TString("");

  // Test geometry -> return non-version file name with 'test' suffix;
  if (IsTestGeometry()) return mDetName->name() + "-test.root";

  // Otherwise compose a default name out of detector name, version and subversion;
  char version[128] = "";
  if (GetVersion() >= 0) 
    snprintf(version, 128-1, "-v%02d.%d", GetVersion(), GetSubVersion());

  switch (GetGeometryType()) {
  case EicGeoParData::Undefined:
    return mDetName->name() + version +         suffix;//".root";
  case EicGeoParData::NoStructure:
    return mDetName->name() + version + "-ns" + suffix;//.root";
  case EicGeoParData::SimpleStructure:
    return mDetName->name() + version + "-ss" + suffix;//.root";
  case EicGeoParData::FullStructure:
    return mDetName->name() + version + "-fs" + suffix;// .root";
  default:
    {
      // Make the compiler happy;
      assert(0); return "";
    }
  } //switch
} // EicGeoParData::GetGeometryFileName()

// ---------------------------------------------------------------------------------------

void EicGeoParData::FinalizeOutput(bool root, bool gdml, bool check) //const
{  
  if (!mRootGeoManager) return;

  // Set up colors and transparency values;
  {
    TIter next( mRootGeoManager->GetListOfVolumes() );

    TGeoVolume *volume;

    while ((volume=(TGeoVolume*)next())) {
      //TString name = volume->GetName();
      //cout << volume->GetName() << endl;

      const std::pair<TString, Color_t> *cpattern = GetColorTable()->AnyMatch(volume->GetName());

      if (cpattern) {
	volume->SetLineColor(cpattern->second);
	volume->SetFillColor(cpattern->second);

	// And check if transparency value is set;
	{
	  const std::pair<TString, Char_t> *tpattern = GetTransparencyTable()->AnyMatch(volume->GetName());

	  // FIXME: well, this 4000-based stuff indeed looks like a hack;
	  if (tpattern) volume->SetFillStyle(4000 + tpattern->second);
	}
      }
      else
	volume->SetVisibility(kFALSE);
    } //while
  }

  // Add configured detector to the top volume, with a proper 3D transformation;
  mWrapperVolume->AddNode(mTopVolume, 0, mTopVolumeTransformation);

  // Check overlap (1um accuracy) and export;
  mRootGeoManager->CloseGeometry();
  //#if _TODAY_
  if (check && mGeometryCheckPrecision) mRootGeoManager->CheckOverlaps(mGeometryCheckPrecision);//0.000001);
  //#endif

  if (root) {
    TFile *outputFile = new TFile(GetGeometryFileName(true).Data(), "RECREATE"); 
    
    if (outputFile) {
      mWrapperVolume->Write();
      
      // Save geometry (mapping) parameters in the same file; 
      Write(mDetName->Name() + "GeoParData");
      
      outputFile->Close();
    } //if
  } //if

  if (gdml) mRootGeoManager->Export(GetGeometryFileName(false).Data());
} // EicGeoParData::FinalizeOutput()

// =======================================================================================

//
//  Service routines
//

// FIXME: unify part of these calls later;

unsigned EicGeoParData::GetDimCore(unsigned group, unsigned what) const 
{
  if (group >= mLogicalVolumeGroups.size()) return 0;
  
  const std::vector<LogicalVolumeGroupProjection*> &coords = 
    mLogicalVolumeGroups[group]->mProjections;
  
  return (what < coords.size() && coords[what] ? coords[what]->mMaxEntryNum : 0);  
} // EicGeoParData::GetDimCore()

// ---------------------------------------------------------------------------------------

unsigned EicGeoParData::GetLogicalCoordCore(unsigned what, ULogicalIndex_t logicalID) const 
{
  unsigned group = GetGroup(logicalID);

  if (group >= mLogicalVolumeGroups.size()) return 0;

  const std::vector<LogicalVolumeGroupProjection*> &coords = 
    mLogicalVolumeGroups[group]->mProjections;
  
  if (what >= coords.size() || !coords[what]) return 0;
  
  EicBitMask<ULogicalIndex_t> *mask = coords[what]->mBitMask;
  
  return ((logicalID >> mask->GetShift()) & mask->GetBitMask());
} // EicGeoParData::GetLogicalCoordCore()

// ---------------------------------------------------------------------------------------

bool EicGeoParData::GetCircularCore(unsigned group, unsigned what) const 
{
  if (group >= mLogicalVolumeGroups.size()) return false;
  
  const std::vector<LogicalVolumeGroupProjection*> &coords = 
    mLogicalVolumeGroups[group]->mProjections;
  
  if (what >= coords.size() || !coords[what]) return false;
  
  return coords[what]->mCircular;  
} // EicGeoParData::GetCircularCore()

// =======================================================================================
//
//  Part used in simulation.C & Co
//

Int_t EicGeoParData::CalculateMappingTableSignatures()
{
  mMaxVolumeLevelNum = 0;

  // Loop through all the maps, calculate bitwise shifts and identify volumes;
  for(int iq=0; iq<mGeantVolumeMaps.size(); iq++)
  {
    EicGeoMap *fmap = mGeantVolumeMaps[iq];
    
    if (fmap->CalculateMappingTableSignature()) return -1;

    // Also use this same loop to get max number of levels;
    if (fmap->GetGeantVolumeLevelNum() > mMaxVolumeLevelNum)
      mMaxVolumeLevelNum = fmap->GetGeantVolumeLevelNum();
  } //for iq

  return 0;
} // EicGeoParData::CalculateMappingTableSignatures()

// =======================================================================================

//
//  Part used in digitization.C & Co
//

void EicGeoParData::InitializeLookupTables()
{
  for(unsigned gr=0; gr<mLogicalVolumeGroups.size(); gr++) {
    LogicalVolumeGroup *vgroup = mLogicalVolumeGroups[gr];
    
    vgroup->mDim3D = 1;
      
    for(unsigned iq=0; iq<_LOGICAL_VOLUME_GROUP_COORD_NUM_; iq++) {
      vgroup->mRealDim[iq] = GetDim(gr,iq) ? GetDim(gr,iq) : 1;
      vgroup->mDim3D *= vgroup->mRealDim[iq];
    } //for iq
      
    vgroup->mLookup = new LogicalVolumeLookupTableEntry[vgroup->mDim3D];
  } //for gr

  TString returnBackPath = gGeoManager->GetPath();

  // Loop through all maps, figure out their "cell" levels (like where "femcTower" is), 
  // loop through all map look-up table entries, construct respective geo node pointers, 
  // get shape, etc info and put all this into the 2D (3D) lookup table;
  for(int iq=0; iq<mGeantVolumeMaps.size(); iq++) {
    int cellLevel = -1;

    const EicGeoMap *fmap = GetMapPtrViaMapID(iq);

    {
      const TString &containerName = fmap->GetSingleSensorContainerVolumeName();

      if (containerName.IsNull()) continue;
      
      for(int lv=0; lv<fmap->GetGeantVolumeLevelNum(); lv++) {
	const GeantVolumeLevel *lptr = fmap->GetGeantVolumeLevelPtr(lv);
	
	if (lptr->GetVolumeName().EqualTo(containerName)) {
	  cellLevel = lv; 
	  break;
	} //if
      } //for lv
      
	// FIXME: remove later (means map is incorrect);
      assert(cellLevel != -1);
      
      // No matching level name -> map is of no interest;
      if (cellLevel == -1) continue;
    }

    // Well, this basically means there were no hits in sensitive volumes
    // served by this map (so assignment in EicDetector::GetNodeMultiIndex()
    // routine has never been reached; in other words, map is useless, skip);
    if (fmap->GetBaseVolumePath()->IsNull()) continue;

    // Loop through all map entries;
    for(UGeantIndex_t ent=0; ent<fmap->GetMappingTableDim(); ent++) {
      ULogicalIndex_t entry = fmap->GetMappingTable()[ent];

      if (entry == ~ULogicalIndex_t(0)) continue;
 
      TString vPath(*fmap->GetBaseVolumePath());
      for(int lv=fmap->GetGeantVolumeLevelNum()-1; lv>=cellLevel; lv--) {
	const GeantVolumeLevel *lptr = fmap->GetGeantVolumeLevelPtr(lv);
	
	// Node ID; this should be a method of course; fix later;
	UGeantIndex_t nID = lptr->GetMaskedBits(ent);
	
	char buffer[128];
	snprintf(buffer, 128-1, "%d", nID);
	vPath += "/" + lptr->GetVolumeName() + "_" + buffer;
      } //for lv

	// This is indeed a CPU-consuming process; may want to do on-the-fly for 
	// the needed cells only;
      gGeoManager->cd(vPath);
      TGeoNode *gNode = gGeoManager->GetCurrentNode();
      LogicalVolumeLookupTableEntry *node = GetLookupTableNode(entry);
      assert(node);
      //node->mLogicalID  = entry;
      node->mGeoNode    = gNode;
      node->mVolumePath = vPath;
      // Yes, gGeoManager->GetCurrentMatrix() is a pointer to whatever 
      // buffer matrix -> reallocate new instead of copy pointer;
      node->mGeoMtx = new TGeoHMatrix(*gGeoManager->GetCurrentMatrix());

      // Add entry to the reversed lookup table;
      mGeantToLogicalLookupTable[gNode] = node;
    } //for ent
  } //for iq

  gGeoManager->cd(returnBackPath);
} // EicGeoParData::InitializeLookupTables()

// ---------------------------------------------------------------------------------------

LogicalVolumeLookupTableEntry *EicGeoParData::GetLookupTableNode(ULogicalIndex_t xy) const 
{
  unsigned group = GetGroup(xy);
  
  if (group >= mLogicalVolumeGroups.size()) return 0;
  
  LogicalVolumeGroup *vgroup = mLogicalVolumeGroups[group];
    
  unsigned offset = GetCoord(0, xy);

  for(unsigned iq=1; iq<_LOGICAL_VOLUME_GROUP_COORD_NUM_; iq++) 
    offset = offset*vgroup->mRealDim[iq] + GetCoord(iq, xy);
  
  return vgroup->mLookup + offset;
} // EicGeoParData::GetLookupTableNode()

// ---------------------------------------------------------------------------------------

LogicalVolumeLookupTableEntry *EicGeoParData::GetLookupTableNode(const TGeoNode *node) const 
{
  // Sanity check;
  if (!node) return 0;

  // No such entry?;
  if (mGeantToLogicalLookupTable.find(node) == mGeantToLogicalLookupTable.end()) return 0;

  return mGeantToLogicalLookupTable.at(node);
} // EicGeoParData::GetLookupTableNode()

// ---------------------------------------------------------------------------------------

ULogicalIndex_t EicGeoParData::GeantMultiToLogicalIndex(ULong64_t multi) const
{
  unsigned mapId = unsigned((multi >> _GEANT_INDEX_BIT_NUM_) & _SERVICE_BIT_MASK_);
  UGeantIndex_t idL = UGeantIndex_t(multi & _GEANT_INDEX_BIT_MASK_);

  return mapId < mGeantVolumeMaps.size() ? 
    mGeantVolumeMaps[mapId]->GeantToLogicalIndex(idL) : ~ULogicalIndex_t(0);
} // EicGeoParData::GeantMultiToLogicalIndex()

// =======================================================================================

//
//  Part used in reconstruction.C & Co
//

bool EicGeoParData::AreNeighbours(ULogicalIndex_t g1, ULogicalIndex_t g2, unsigned maxLinearDist, 
				  unsigned maxChebyshevDist) const 
{
  if (GetGroup(g1) != GetGroup(g2)) return false;

  int x1       [3] = {GetX(g1),       GetY(g1),       GetZ(g1)};
  int x2       [3] = {GetX(g2),       GetY(g2),       GetZ(g2)};
  unsigned dim [3] = {GetDimX(),      GetDimY(),      GetDimZ()}, distChebyshev = 0;
  bool circ    [3] = {GetCircularX(), GetCircularY(), GetCircularZ()};

  // Loop through all 3 dimensions and drop out as soon as any of the 
  // specified limits are exceeded;
  for(unsigned iq=0; iq<_LOGICAL_VOLUME_GROUP_COORD_NUM_; iq++)
  {
    // This dimension is of no interest; actually dim=1 should also be skipped?;
    if (!dim[iq]) continue;

    UInt_t dist = abs(x2[iq] - x1[iq]);
    // If this dimension is circular, check other option as well 
    // and select smaller one; whould be a range check here?; fix later;
    if (circ[iq])
    {
      UInt_t cdist = dim[iq] - dist;
      if (cdist < dist) dist = cdist;
    } //if

    // Linear distance is too large; NB: in principle it is not necessary to have 
    // maxLinearDist=1; nothing prevents from say searching for low-energy-deposit 
    // clusters "far enough" from the main blob;
    if (dist > maxLinearDist) return false;

    // No this check per default;
    if (maxChebyshevDist)
    {
      distChebyshev += dist;
    
      // 2D (or 3D) distance is too large; idea is to be able to suppress 2D-diagonal
      // neighbourships for 2D-maps as well as 3D-diagonal (or even 2D-diagonal) ones 
      // for 3D-maps;
      if (distChebyshev > maxChebyshevDist) return false;
    } //if
  } //for iq

  // Passed all checks -> they are neighbors;
  return true;
} // EicGeoParData::AreNeighbours()

// =======================================================================================

void EicGeoParData::Print(const char *option) const
{
  // Figure out class name and version;
  printf("\nClass name: %s (v.%d); object name: %s\n\n", 
	 ClassName(), Class()->GetClassVersion(), GetName());

  // Author;
  printf("Created by %s\n\n", mAuthor.Data());

  // Creation time;
  mTimeStamp.Print("l");

  // Version, subversion and comment;
  if (mVersion != -1) printf("\nVersion: v%02d.%d; comment: %s\n\n", mVersion, mSubVersion, 
			     mComment.IsNull() ? "-" : mComment.Data());

  // Class() method returns EicGeoParData instead of say VstGeoParData; so call
  // gROOT->GetClass() instead; need to verify that this will work under all circumstances;
  //TList *dataList = Class()->GetListOfRealData();
  TList *dataList = gROOT->GetClass(ClassName())->GetListOfRealData();
  TIter next(dataList);

  // For now handle POD variables of basic types;
  printf("  Basic type variables:\n");
  printf("  ---------------------\n\n");

  TRealData *data;
  while ((data=(TRealData*)next())) {
    TDataMember *member = data->GetDataMember();

    // These guys are for sure of no interest;
    if (!member->IsPersistent()) continue;

    // Deal with basic types only for now;
    if (!member->IsBasic())      continue;

    // Not really interested in these guys either;
    if (!strcmp(data->GetName(), "fUniqueID") || !strcmp(data->GetName(), "fBits"))      continue;
    if (!strcmp(data->GetName(), "mVersion") || !strcmp(data->GetName(), "mSubVersion")) continue;
    if (!strcmp(data->GetName(), "mTimeStamp.fSec") || 
	!strcmp(data->GetName(), "mTimeStamp.fNanoSec")) 
      continue;

    printf("%-30s (%-10s):", data->GetName(), member->GetFullTypeName());

    Long_t offset = member->GetOffset();

    if (!strcmp(member->GetFullTypeName(), "Double_t"))
      printf(" %f\n", *(Double_t*)((char*)this + offset));
    else if (!strcmp(member->GetFullTypeName(), "Int_t"))
      printf(" %d\n", *(Int_t*)((char*)this + offset));
    else if (!strcmp(member->GetFullTypeName(), "UInt_t"))
      printf(" %u\n", *(UInt_t*)((char*)this + offset));
    else if (!strcmp(member->GetFullTypeName(), "Bool_t"))
      printf(" %s\n", *(Bool_t*)((char*)this + offset) ? "true" : "false");
  } //while  
} // EicGeoParData::Print()

/* ========================================================================== */

TVector3 LocalToMaster    (const TGeoMatrix *mtx, const TVector3& local)
{
  double xlocal[3] = {local.X(), local.Y(), local.Z()}, xmaster[3];

  mtx->LocalToMaster(xlocal, xmaster);

  return TVector3(xmaster[0], xmaster[1], xmaster[2]);
} // LocalToMaster()

// ---------------------------------------------------------------------------------------

TVector3 LocalToMasterVect(const TGeoMatrix *mtx, const TVector3& local)
{
  double xlocal[3] = {local.X(), local.Y(), local.Z()}, xmaster[3];

  mtx->LocalToMasterVect(xlocal, xmaster);

  return TVector3(xmaster[0], xmaster[1], xmaster[2]);
} // LocalToMasterVect()

// ---------------------------------------------------------------------------------------

TVector3 MasterToLocal    (const TGeoMatrix *mtx, const TVector3& master)
{
  double xmaster[3] = {master.X(), master.Y(), master.Z()}, xlocal[3];

  mtx->MasterToLocal(xmaster, xlocal);
  
  return TVector3(xlocal[0], xlocal[1], xlocal[2]);
} // MasterToLocal()

// ---------------------------------------------------------------------------------------

TVector3 MasterToLocalVect(const TGeoMatrix *mtx, const TVector3& master)
{
  double xmaster[3] = {master.X(), master.Y(), master.Z()}, xlocal[3];

  mtx->MasterToLocalVect(xmaster, xlocal);
  
  return TVector3(xlocal[0], xlocal[1], xlocal[2]);
} // MasterToLocalVect()

// =======================================================================================

ClassImp(LogicalVolumeGroupProjection)
ClassImp(LogicalVolumeGroup)
ClassImp(SourceFile)
ClassImp(EicGeoParData)
ClassImp(LogicalVolumeLookupTableEntry)

