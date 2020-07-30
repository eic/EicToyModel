
    {
      VstGeoParData vst;
      vst.SetGeometryType(EicGeoParData::SimpleStructure);

      auto ibcell = new MapsMimosaAssembly();
      
      //vst.AddBarrelLayer(ibcell, 6*12, 14, 6*23.4 * etm::mm, 14.0, 0.0);
      vst.AddBarrelLayer(ibcell, 4*20, 14, 4*39.3 * etm::mm, 14.0, 0.0);

      auto mid = eic->mid()->get("TRACKER")->GetG4Volume();
      vst.PlaceG4Volume(mid, true, 0, new G4ThreeVector(0, 0, 0));
    }
