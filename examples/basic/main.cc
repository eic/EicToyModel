
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4VModularPhysicsList.hh"
#include "G4GDMLParser.hh"
#include "G4GDMLReadStructure.hh"
#include "G4AssemblyVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"

#include <TROOT.h>
#include <TFile.h>
#include <TColor.h>

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

    // The easiest: ask the model to build its own IR world;
    auto expHall_phys = eic->ConstructG4World();
    expHall_phys->GetLogicalVolume()->SetVisAttributes(G4VisAttributes::Invisible);

    // Construct the integration volumes geometry, internally;
    eic->Construct();

    // Place them as G4 volumes into the IR world volume all at once ...
    eic->PlaceG4Volumes(expHall_phys);
    // ... or just a single "EmCal" volume of the h-endcap, under "MyEmCal" name; 
    /*auto emcal =*/ //eic->fwd()->get("EmCal")->PlaceG4Volume(expHall_phys, "MyEmCal");

    //eic->mid()->get("TPC")   ->PlaceG4Volume(expHall_phys);
    //eic->vtx()->get("Si Tracker")->PlaceG4Volume(expHall_phys);
    //auto trd = eic->bck()->get("TRD", 0)   ->PlaceG4Volume(expHall_phys);
    //auto trd = eic->bck()->get("Cherenkov")   ->PlaceG4Volume(expHall_phys);

#if _OK_
    {
      auto GDMLRS = new G4GDMLReadStructure();
      G4GDMLParser gdmlParser(GDMLRS);
      //gdmlParser.SetOverlapCheck(true);
      // FIXME: may as well import from the same ROOT file ("VC.GDML" TObjString);
      gdmlParser.Read("../../../build/sandbox.vc.gdml", false);
      G4AssemblyVolume *avol = GDMLRS->GetAssembly("VC.ASSEMBLY");
      if (avol) {
	G4VisAttributes* visAttg = new G4VisAttributes();
	visAttg->SetColor(.5, .5, .5);
	visAttg->SetVisibility(true);
	visAttg->SetForceWireframe(false);
	visAttg->SetForceSolid(true);
	
	std::vector<G4VPhysicalVolume *>::iterator it = avol->GetVolumesIterator();
	for (unsigned int i = 0; i < avol->TotalImprintedVolumes(); i++) {
	  //printf("@@@ %30s ... %d\n", (*it)->GetName().data(), (*it)->GetLogicalVolume()->GetNoDaughters());
	  (*it)->GetLogicalVolume()->SetVisAttributes(visAttg);
	  //InsertVolumes(*it);
	  ++it;
	}
	
	//G4RotationMatrix *g4rot = new G4RotationMatrix();
	G4ThreeVector g4vec;
	avol->MakeImprint(expHall_phys->GetLogicalVolume(), g4vec,
			  new G4RotationMatrix(), 0, true);
      } //if
    }
#endif

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
  if (!EicToyModel::Import(argv[1])) return -1;

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
  //++UImanager->ApplyCommand("/vis/drawVolume ! ! ! -box m   0 10 0 10 -10 10");
  //UImanager->ApplyCommand("/vis/drawVolume ! ! ! -box m -10 10 0 10 -10 10");
  UImanager->ApplyCommand("/vis/drawVolume");
  //UImanager->ApplyCommand("/vis/scene/add/axes 0 0 0 1 m");
  UImanager->ApplyCommand("/vis/viewer/set/background white");
  UImanager->ApplyCommand("/vis/viewer/zoom 2.0");
  //UImanager->ApplyCommand("/geometry/test/run");

  UImanager->ApplyCommand("/vis/viewer/clearCutawayPlanes");     
  UImanager->ApplyCommand("/vis/viewer/addCutawayPlane 0 0 7 m 1  0 0");
  UImanager->ApplyCommand("/vis/viewer/addCutawayPlane 0 0 7 m 0 -1 0");

  ui->SessionStart();

  delete ui; delete visManager; delete runManager;

  return 0;
} // main()

// ---------------------------------------------------------------------------------------
