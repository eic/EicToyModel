
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VModularPhysicsList.hh"

#include <EicToyModel.h>
#include <GemGeoParData.h>
#include <MuMegasGeoParData.h>
#include <VstGeoParData.h>

// ---------------------------------------------------------------------------------------

class BasicDetectorConstruction : public G4VUserDetectorConstruction {
public:
  BasicDetectorConstruction() {};
  ~BasicDetectorConstruction() {};

  G4VPhysicalVolume* Construct() {
    auto eic = EicToyModel::Instance();

    // The easiest: ask the model to build its own IR world;
    auto expHall_phys = eic->ConstructG4World();
    if (!expHall_phys) exit(0);
    expHall_phys->GetLogicalVolume()->SetVisAttributes(G4VisAttributes::Invisible);

    // Construct the integration volumes geometry, internally; see scripts/eicroot.C
    // for the volume composition;
    eic->Construct();

    // Place them as G4 volumes into the IR world volume all at once ...
    //eic->PlaceG4Volumes(expHall_phys);
    eic->DefineG4World(expHall_phys->GetLogicalVolume());

    // Vertex tracker;
    {
      VstGeoParData vst;
      //vst.SetGeometryType(EicGeoParData::NoStructure);
      //vst.WithEnforcementBrackets();
      //vst.WithExternalPipes();

      auto ibcell = new MapsMimosaAssembly();

      // Compose barrel layers; parameters are:
      //  - cell assembly type;
      //  - number of staves in this layer;
      //  - number of chips in a stave;
      //  - chip center installation radius;
      //  - additional stave slope around beam line direction; [degree];
      //  - layer rotation around beam axis "as a whole"; [degree];
      //
      vst.AddBarrelLayer(ibcell, 3*12,  9, 3*23.4 * etm::mm, 12.0, 0.0);
      
      auto mid = eic->mid()->get("TRACKER")->GetG4Volume();
      vst.PlaceG4Volume(mid, true, 0, new G4ThreeVector(0, 0, 0));
    }

    // Forward GEM tracker module(s);
    {
      GemGeoParData fgt("FGT");
      auto sbs = new GemModule();

      // Compose sectors; parameters are: 
      //   - layer description (obviously can mix different geometries);
      //   - azimuthal segmentation;
      //   - gas volume center radius;
      //   - Z offset from 0.0 (default);
      //   - azimuthal rotation from 0.0 (default);
      fgt.AddWheel(sbs, 12, 420.0 * etm::mm, -50.0 * etm::mm, 0);
      fgt.AddWheel(sbs, 12, 420.0 * etm::mm,  50.0 * etm::mm, 0);

      auto fwd = eic->fwd()->get("MPGD")   ->GetG4Volume();
      // Build ROOT geometry, convert it to GEANT geometry, place into the mother volume;
      fgt.PlaceG4Volume(fwd);
    }
    // Backward GEM tracker module(s);
    {
      GemGeoParData bgt("BGT");
      GemModule *sbs = new GemModule();

      bgt.AddWheel(sbs, 12, 420.0 * etm::mm,   0.0 * etm::mm);

      bgt.SetTransparency(50);

      auto bck = eic->bck()->get("TRD")    ->GetG4Volume();
      bgt.PlaceG4Volume(bck, false, 0, new G4ThreeVector(0, 0, 50 * g4::mm));
    }

    // Micromegas central tracker barrels;
    {
      MuMegasGeoParData mmt("MMT");
      MuMegasLayer *layer = new MuMegasLayer();
      // See other MuMegasLayer class POD entries in include/MuMegasGeoParData.h;
      //layer->SetDoubleVariable("mOuterFrameWidth", 50 * etm::mm);
	      
      // Compose barrel layers; parameters are: 
      //   - layer description (obviously can mix different geometries);
      //   - length along Z;
      //   - segmentation in Z;
      //   - radius;
      //   - segmentation in phi;
      //   - Z offset from 0.0 (default);
      //   - azimuthal rotation from 0.0 (default);
      mmt.AddBarrel(layer, 600. * etm::mm, 2, 300. * etm::mm, 3, 0.0, 0.0);
      //mmt.AddBarrel(layer,1800. * etm::mm, 4, 800. * etm::mm, 6, 0.0, 0.0);
      
      mmt.SetTransparency(50);

      auto mid = eic->mid()->get("TRACKER")->GetG4Volume();
      mmt.PlaceG4Volume(mid, false, 0, new G4ThreeVector(0, 0, 20 * g4::mm));
    }

    return expHall_phys;
  };
};

// ---------------------------------------------------------------------------------------

class BasicPhysicsList : public G4VModularPhysicsList {
 public:
  BasicPhysicsList() {};
  ~BasicPhysicsList() {};
};

// ---------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
  if (argc != 3) {
    printf("\n\n   usage: %s <EicToyModel-root-file-name> <EicRoot-media-file-name>\n\n\n", argv[0]);
    return -1;
  } //if

  // Import the ROOT file with an "EicToyModel" singleton class instance; 
  if ( !EicToyModel::Import(         argv[1])) return -1;
  // Import EicRoot media.geo file;
  if (EicGeoParData::ImportMediaFile(argv[2])) return -1;

  // The rest is a usual GEANT stuff;
  G4RunManager *runManager = new G4RunManager;
  runManager->SetUserInitialization(new BasicDetectorConstruction());
  runManager->SetUserInitialization(new BasicPhysicsList());
  runManager->Initialize();

  G4VisManager *visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  G4UImanager *UImanager = G4UImanager::GetUIpointer(); 
   
  G4UIExecutive *ui = new G4UIExecutive(argc, argv);
  UImanager->ApplyCommand("/vis/open OGL 600x600-0+0");
  // Define a 3D cutaway view; 
  UImanager->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 110. 150.");
  UImanager->ApplyCommand("/vis/viewer/set/lightsThetaPhi    110. 150.");
  UImanager->ApplyCommand("/vis/drawVolume");
  UImanager->ApplyCommand("/vis/scene/add/axes 0 0 0 1 m");
  UImanager->ApplyCommand("/vis/viewer/set/background white");
  UImanager->ApplyCommand("/vis/viewer/zoom 2.0");
  //UImanager->ApplyCommand("/geometry/test/run");

  //UImanager->ApplyCommand("/vis/viewer/clearCutawayPlanes");     
  //UImanager->ApplyCommand("/vis/viewer/addCutawayPlane 0 0 7 m 1  0 0");
  //UImanager->ApplyCommand("/vis/viewer/addCutawayPlane 0 0 7 m 0 -1 0");

  ui->SessionStart();

  delete ui; delete visManager; delete runManager;

  return 0;
} // main()

// ---------------------------------------------------------------------------------------
