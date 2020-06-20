
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4VModularPhysicsList.hh"

#include <TFile.h>

#include <EicToyModel.h>

// ---------------------------------------------------------------------------------------

class BasicDetectorConstruction : public G4VUserDetectorConstruction {
public:
  BasicDetectorConstruction() {};
  ~BasicDetectorConstruction() {};

  G4VPhysicalVolume* Construct() {
    auto eic = EicToyModel::Instance();

    // Print IR dimensions;
    printf("\n\nIR box size: Z +/- %.2f [cm], R ~ %.2f [cm]\n\n", 
	   eic->GetIrRegionLength()/2, eic->GetIrRegionRadius());

    // The easiest: ask the model to build its IR world;
    auto expHall_phys = eic->ConstructG4World();
    expHall_phys->GetLogicalVolume()->SetVisAttributes(G4VisAttributes::Invisible);

    // Construct the integration volumes geometry, internally;
    eic->Construct();

    // Place them as G4 volumes into the IR world volume all at once ...
    eic->PlaceG4Volumes(expHall_phys);
    // ... or just a single "EmCal" volume of the h-endcap, under "MyEmCal" name; 
    /*auto emcal =*/ //eic->fwd()->get("EmCal")->PlaceG4Volume(expHall_phys, "MyEmCal");

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
  if (argc != 2) {
    printf("\n\n   usage: %s <EicToyModel-root-file-name>\n\n\n", argv[0]);
    return -1;
  } //if

  // Import the ROOT file with an "EicToyModel" singleton class instance; 
  if (EicToyModel::Import(argv[1])) return -1;

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
  UImanager->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 60. 20.");
  UImanager->ApplyCommand("/vis/drawVolume ! ! ! -box m 0 10 0 10 -10 10");
  //--UImanager->ApplyCommand("/vis/drawVolume");
  UImanager->ApplyCommand("/vis/scene/add/axes 0 0 0 1 m");
  UImanager->ApplyCommand("/vis/viewer/set/background white");
  UImanager->ApplyCommand("/vis/viewer/zoom 2.0");
  //++UImanager->ApplyCommand("/geometry/test/run");
  ui->SessionStart();

  delete ui; delete visManager; delete runManager;

  return 0;
} // main()

// ---------------------------------------------------------------------------------------
