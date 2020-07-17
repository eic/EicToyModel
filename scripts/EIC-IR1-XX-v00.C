
{
  auto eic = new EicToyModel();

  // Shift IP if needed; define canvas width; request eta=0 line in the drawing; set name;
  eic->ip(0.0 * etm::cm)->width(1200)->AddEtaLine(0.0)->SetName("example");
  // Define acceptance ranges and the vacuum chamber design;
  eic->acceptance(-4.2, -1.0, 1.1, 4.2);
  eic->SetAzimuthalSegmentation(12);
  eic->DefineVacuumChamber(new vc2020_03_20());

  // Vertex tracker;
  {
    auto vtx = eic->vtx(); vtx->offset(0.0 * etm::cm);

    vtx->add("Si Tracker",20 * etm::cm);
  }

  // Barrel;
  {
    auto mid = eic->mid(); mid->offset( 20 * etm::cm);
      
    mid->add("TRACKER",   75 * etm::cm);
    mid->add("Cherenkov", 25 * etm::cm);
    mid->add("EmCal",     30 * etm::cm);
    mid->add("Cryostat",  40 * etm::cm);
    mid->add("HCal",     120 * etm::cm)->brick();
  }

  // Hadron-going endcap;
  {
    auto fwd = eic->fwd(); fwd->offset(130 * etm::cm);

    fwd->add("TRACKER",   15 * etm::cm)->brick();
    fwd->marker();

    fwd->add("HM RICH",  100 * etm::cm)->trim(0.8, 1.0);
    for(unsigned nn=0; nn<2; nn++)
      fwd->add("TRD",     15 * etm::cm)->brick();

    fwd->add("Preshower",  5 * etm::cm);
    fwd->add("EmCal",     40 * etm::cm);
    fwd->add("HCal",     105 * etm::cm);
  } 

  // Electron-going endcap;
  {
    auto bck = eic->bck(); bck->offset(120 * etm::cm);

    bck->add("TRACKER",   15 * etm::cm)->brick();
    for(unsigned nn=0; nn<3; nn++)
      bck->add("TRD",     15 * etm::cm)->brick();
    bck->marker();

    bck->add("Cherenkov", 25 * etm::cm);
    bck->add("Preshower",  5 * etm::cm)->brick();
    bck->add("TOF",        5 * etm::cm)->brick();
    bck->add("EmCal",     50 * etm::cm);
    bck->add("HCal",     105 * etm::cm);
  }

  // Declare eta boundary configuration;
  {
    eic->vtx()->get("Si Tracker")->stretch(eic->bck()->get("Cherenkov"));
    eic->vtx()->get("Si Tracker")->stretch(eic->fwd()->get("HM RICH"));

    eic->mid()->get("TRACKER")   ->stretch(eic->bck()->get("TRACKER"));
    eic->mid()->get("TRACKER")   ->stretch(eic->fwd()->get("TRACKER"));
    eic->mid()->get("HCal")      ->stretch(eic->bck()->get("HCal"));
    eic->mid()->get("HCal")      ->stretch(eic->fwd()->get("HCal"));
    eic->mid()->get("Cryostat")  ->stretch(eic->bck()->get("HCal"));
    eic->mid()->get("EmCal")     ->stretch(eic->bck()->get("EmCal"));

    eic->fwd()->get("HCal")      ->stretch(eic->mid()->get("HCal"), 80 * etm::cm);

    eic->bck()->get("HCal")      ->stretch(eic->mid()->get("HCal"), 80 * etm::cm);
  }

  // Beautify picture a little bit;
  eic->ApplyStandardTrimming();

  // Draw horizontal cross cut view; write the .root file out;
  //eic->mirror();
  eic->hdraw();
  eic->write();
  //eic->Export("example.stp");
} 
