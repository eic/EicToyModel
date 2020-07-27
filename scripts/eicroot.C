{
  auto eic = new EicToyModel();

  // Define canvas width; request eta=0 line in the drawing; set name;
  eic->width(1500)->AddEtaLine(0.0)->SetName("eicroot");
  // Define acceptance ranges; go simple: close to the beam line, and no vacuum chamber cutaway;
  eic->acceptance(-6.2, -1.0, 1.2, 6.2);

  // Barrel;
  {
    auto mid = eic->mid(); mid->offset(  5 * etm::cm);
      
    mid->add("TRACKER",100 * etm::cm);
  }

  // Hadron-going endcap;
  {
    auto fwd = eic->fwd(); fwd->offset(120 * etm::cm);

    fwd->add("MPGD",    30 * etm::cm)->brick();
  } 

  // Electron-going endcap;
  {
    auto bck = eic->bck(); bck->offset(120 * etm::cm);

    bck->add("TRD",     15 * etm::cm)->brick();
  } 

  // Declare eta boundary configuration;
  {
    eic->mid()->get("TRACKER")->stretch(eic->bck()->get("TRD"));
    eic->mid()->get("TRACKER")->stretch(eic->fwd()->get("MPGD"));
  }

  // Draw vertical cross cut view;
  eic->vdraw();

  // Write eicroot.root file with the configuration out;
  eic->write();
} 
