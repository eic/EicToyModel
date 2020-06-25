
//#define _SPLIT_

{
  auto eic = new EicToyModel();

  // Shift IP if needed; define canvas width; request eta=0 line in the drawing; set name;
  eic->ip(-50.0)->width(1500)->AddEtaLine(0.0)->SetName("tracking-example");
  // Define acceptance ranges and the vacuum chamber design;
  eic->acceptance(-4.2, -1.0, 1.2, 4.2);
  eic->DefineVacuumChamber(new vc2020_03_20());

  // Vertex tracker;
  {
    // BUG: G4 event display does not work well for <3.2cm; 
    auto vtx = eic->vtx(); vtx->offset(3.2 * etm::cm);

    vtx->add("Si Tracker",17 * etm::cm);
  }

  // Barrel;
  {
    auto mid = eic->mid(); mid->offset( 20 * etm::cm);
      
    mid->add("Si Tracker",75 * etm::cm);

    mid->add("Cherenkov", 10 * etm::cm)->brick();
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

#ifdef _SPLIT_
    for(unsigned nn=0; nn<3; nn++)
      fwd->add("MPGD",     5 * etm::cm)->brick();
#else
    fwd->add("MPGD",      15 * etm::cm)->trim(0.8, 1.0);
#endif
    fwd->marker();

    fwd->add("HM RICH",  110 * etm::cm)->trim(0.8, 1.0);
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

#ifdef _SPLIT_
    for(unsigned nn=0; nn<3; nn++)
      bck->add("MPGD",     5 * etm::cm)->brick();

    for(unsigned nn=0; nn<3; nn++)
      bck->add("TRD",     15 * etm::cm)->brick();
#else
    bck->add("MPGD",      60 * etm::cm)->trim(1.0, 1.0);
#endif
    bck->marker();

    bck->add("Cherenkov", 25 * etm::cm);
    bck->add("Preshower",  5 * etm::cm)->brick();
    bck->add("EmCal",     35 * etm::cm);
    bck->add("HCal",     105 * etm::cm)->brick();
  }

  // Declare eta boundary configuration;
  {
    eic->vtx()->get("Si Tracker")->stretch(eic->bck()->get("Cherenkov"));
    eic->vtx()->get("Si Tracker")->stretch(eic->fwd()->get("HM RICH"));

    eic->mid()->get("Si Tracker")->stretch(eic->bck()->get("MPGD"));
    eic->mid()->get("Si Tracker")->stretch(eic->fwd()->get("MPGD"));

    eic->mid()->get("HCal")      ->stretch(eic->bck()->get("HCal"));
    eic->mid()->get("HCal")      ->stretch(eic->fwd()->get("HCal"));

    eic->mid()->get("Cryostat")  ->stretch(eic->bck()->get("HCal"));
    eic->mid()->get("EmCal")     ->stretch(eic->bck()->get("EmCal"));
  }

  // Beautify picture a little bit;
  eic->ApplyStandardTrimming();

  // Draw horizontal cross cut view; write the .root file out;
  eic->hdraw();
  eic->write();
} 
