void MyDetectorDetector::ConstructMe(G4LogicalVolume *logicWorld)
{
  // Import ROOT file with an "EicToyModel" singleton class instance;                                                                        
  auto eic = EicToyModel::Import("example.root");

   // Construct the integration volumes geometry, internally;                                                                                    
  eic->Construct();

  // Place them as G4 volumes into the IR world volume all at once; 
  eic->PlaceG4Volumes(logicWorld);

  // Singleton instance, no mess;
  auto eic = EicToyModel::Instance();

  // Get pointer to a particular G4VPhysicalVolume;
  auto hcal = eic->fwd()->get("HCal")->GetG4Volume();

  // ...

  G4VPhysicalVolume *phy = new G4PVPlacement(
      rotm,
      G4ThreeVector(m_Params->get_double_param("place_x") * cm,
                    m_Params->get_double_param("place_y") * cm,
                    m_Params->get_double_param("place_z") * cm),
      //logical, "MyDetector", logicWorld, 0, false, OverlapCheck());
      logical, "MyDetector", hcal->GetLogicalVolume(), 0, false, OverlapCheck());

  // ...
}
