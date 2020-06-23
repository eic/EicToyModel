{
  auto eic = new EicToyModel();

  // Shift IP if needed; define canvas width; request eta=0 line in the drawing; set name;
  eic->ip(-50.0)->width(1500)->AddEtaLine(0.0)->SetName("sandbox");
  // Define acceptance ranges and the vacuum chamber design;
  eic->acceptance(-7.2, -1.0, 1.2, 4.2);
  eic->DefineVacuumChamber(new vc2020_03_20());
  //eic->SetAzimuthalSegmentation(16);

  // Vertex tracker;
  {
    auto vtx = eic->vtx(); vtx->offset(  3.2 * etm::cm);

    vtx->add("Si Tracker",17 * etm::cm);
  }

  // Barrel;
  {
    auto mid = eic->mid(); mid->offset( 20 * etm::cm);
      
    mid->add("TPC",       60 * etm::cm);
    mid->add("Cherenkov", 25 * etm::cm)->brick();
    mid->add("MPGD",       5 * etm::cm)->brick();
    mid->add("TOF",        5 * etm::cm)->brick();
    mid->add("Preshower",  5 * etm::cm)->brick();
    mid->add("EmCal",     30 * etm::cm);
    mid->add("Cryostat",  40 * etm::cm);
    mid->add("HCal",     120 * etm::cm);
  }

  // Hadron-going endcap;
  {
    auto fwd = eic->fwd(); fwd->offset(150 * etm::cm);
    //auto fwd = eic->fwd(); fwd->offset(20 * etm::cm);

    for(unsigned nn=0; nn<3; nn++)
      fwd->add("MPGD",     5 * etm::cm)->brick();
    fwd->marker();
    fwd->add("HM RICH",  120 * etm::cm)->trim(0.8, 1.0);
    for(unsigned nn=0; nn<3; nn++)
      fwd->add("TRD",     15 * etm::cm)->brick();
    fwd->add("TOF",        5 * etm::cm);
    fwd->add("Preshower",  5 * etm::cm);
    fwd->add("EmCal",     35 * etm::cm);
    fwd->add("HCal",     105 * etm::cm);
  } 

  // Electron-going endcap;
  {
    auto bck = eic->bck(); bck->offset(120 * etm::cm);

    for(unsigned nn=0; nn<3; nn++)
      bck->add("MPGD",     5 * etm::cm)->brick();

    for(unsigned nn=0; nn<3; nn++)
      bck->add("TRD",     15 * etm::cm)->brick();
    bck->marker();
    bck->add("TOF",        5 * etm::cm);
    bck->add("Cherenkov", 25 * etm::cm);
    bck->add("Preshower",  5 * etm::cm);
    bck->add("EmCal",     35 * etm::cm);
    bck->add("HCal",     105 * etm::cm);
  }

  // Declare eta boundary configuration;
#if 1
  {
    eic->vtx()->get("Si Tracker")->stretch(eic->bck()->get("TOF"));
    eic->vtx()->get("Si Tracker")->stretch(eic->fwd()->get("HM RICH"));

    eic->mid()->get("TPC")       ->stretch(eic->bck()->get("MPGD"));
    eic->mid()->get("TPC")       ->stretch(eic->fwd()->get("MPGD"));

    eic->mid()->get("HCal")->stretch(eic->bck()->get("HCal"), 50 * etm::cm);
    eic->mid()->get("HCal")->stretch(eic->fwd()->get("HCal"), 50 * etm::cm);
  }
#endif

  // Beautify picture a little bit;
  eic->ApplyStandardTrimming();
  // Request flat field regions to be drawn in the endcaps;
  eic->DrawFlatFieldLines(-3.0)->DrawFlatFieldLines(3.0);

  // 
  eic->hdraw();
  eic->write(true);
  //eic->Export("sandbox.stp");
  //eic->ExportVacuumChamber();
} 
