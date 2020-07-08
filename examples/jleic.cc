void JLeicDetectorConstruction::Create_ce_Endcap(JLeicDetectorConfig::ce_Endcap_Config cfg)
{                                                                                                                                
  // ...

  // Import ROOT file with an "EicToyModel" singleton class instance;                                                                        
  auto eic = EicToyModel::Import("example.root");

   // Construct the integration volumes geometry, internally;                                                                                    
  eic->Construct();

  // Place them as G4 volumes into the IR world volume all at once;                                                                              
  eic->PlaceG4Volumes(World_Phys);

  // Get pointer to a particular G4VPhysicalVolume;
  ce_ENDCAP_GVol_Phys = eic->bck()->get("HCal")->GetG4Volume();

  // ...
}
