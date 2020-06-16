
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4Box.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PVPlacement.hh"
#include "G4VModularPhysicsList.hh"

#include <TFile.h>

#include <EicToyModel.h>

// ---------------------------------------------------------------------------------------

class EtmDetectorConstruction : public G4VUserDetectorConstruction {
public:
  EtmDetectorConstruction() {};
  ~EtmDetectorConstruction() {};

  G4VPhysicalVolume* Construct() {
    auto eic = EicToyModel::Instance();
    auto air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

    double cff = cm/etm::cm;
    // Define the "experimental hall"; units are TGeo [cm], as stored;
    printf("\n\nIR box size: Z +/- %.2f [cm], R ~ %.2f [cm]\n\n", 
	   eic->GetIrRegionLength()/2, eic->GetIrRegionRadius());
    auto expHall_box = new G4Box("World", 
				 cff*eic->GetIrRegionLength()/2, 
				 cff*eic->GetIrRegionRadius(),
				 cff*eic->GetIrRegionRadius());
    auto expHall_log = new G4LogicalVolume(expHall_box, air, "World", 0, 0, 0);
    expHall_log->SetVisAttributes(G4VisAttributes::Invisible);

    // Construct the integration volumes geometry;
    eic->Construct();

    // Place them into the IR world volume all at once;
    eic->PlaceG4Volumes(expHall_log);
    // Place just a single "EmCal" volume of the h-endcap, under "MyEmCal" name; 
    //auto emcal = eic->fwd()->get("EmCal")->PlaceG4Volume(expHall_log, "MyEmCal");

    //eic->bck()->get("Preshower")->PlaceG4Volume(expHall_log);

    return new G4PVPlacement(0, G4ThreeVector(), expHall_log, "World", 0, false, 0);
  };
};

// ---------------------------------------------------------------------------------------

class EtmPhysicsList : public G4VModularPhysicsList {
 public:
  EtmPhysicsList() {};
  ~EtmPhysicsList() {};
};

// ---------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
  if (argc != 2) {
    printf("\n\n   usage: %s <EicToyModel-root-file-name>\n\n\n", argv[0]);
    return -1;
  } //if

  // Import the ROOT file with an "EicToyModel" singleton class instance; 
  TFile fin(argv[1]);
  auto eic = dynamic_cast<EicToyModel *>(fin.Get("EicToyModel"));
  if (!eic) {
    printf("Wrong file format: no EicToyModel instance found!\n\n\n");
    return -1;
  } //if
  fin.Close();

  G4RunManager *runManager = new G4RunManager;
  runManager->SetUserInitialization(new EtmDetectorConstruction());
  runManager->SetUserInitialization(new EtmPhysicsList());
  runManager->Initialize();

  G4VisManager *visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  G4UImanager *UImanager = G4UImanager::GetUIpointer(); 
   
  G4UIExecutive *ui = new G4UIExecutive(argc, argv);
  UImanager->ApplyCommand("/vis/open OGL 600x600-0+0");
  // Define a 3D cutaway view; 
  UImanager->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 60. 20.");
  UImanager->ApplyCommand("/vis/drawVolume ! ! ! -box m 0 10 0 10 -10 10");
  UImanager->ApplyCommand("/vis/scene/add/axes 0 0 0 1 m");
  UImanager->ApplyCommand("/vis/viewer/set/background white");
  UImanager->ApplyCommand("/vis/viewer/zoom 2.0");
  ui->SessionStart();

  delete ui; delete visManager; delete runManager;

  return 0;
} // main()

// ---------------------------------------------------------------------------------------
