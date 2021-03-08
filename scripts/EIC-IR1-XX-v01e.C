
{
  auto eic = new EicToyModel();

  // Shift IP if needed; define canvas width; request eta=0 line in the drawing; set name;
  eic->ip(0.0 * etm::cm)->width(1200)->AddEtaLine(0.0)->SetName("EIC-IR1-XX-v01e");
  eic->ir(1020.0 * etm::cm, 420.0*etm::cm);
  // Define acceptance ranges and the vacuum chamber design;
  eic->acceptance(-4.2, -1.0, 1.1, 4.2);
  eic->SetAzimuthalSegmentation(12);
  eic->DefineVacuumChamber(new vc2020_03_20());
  //eic->DrawIP6boundaries();
  //eic->UseDetectorHighlighting();

  // Vertex tracker;
  {
    auto vtx = eic->vtx(); vtx->offset(0.0 * etm::cm);

    vtx->add("Si Tracker",20 * etm::cm);//->highlight(1.0);
  }

  // Barrel;
  {
    auto mid = eic->mid(); mid->offset( 20 * etm::cm);
      
    mid->add("TPC",       80 * etm::cm);//->highlight();
    mid->add("DIRC",      20 * etm::cm)->trim(1.0, 0.3);//->highlight();
    mid->add("EmCal",     40 * etm::cm);//->highlight();
    mid->add("Cryostat",  60 * etm::cm)->brick();//trim(1.0, 0.01);
    mid->add("HCal",     120 * etm::cm)->trim(0.0, 1.00);//->highlight();;
  }

  // Hadron-going endcap;
  {
    auto fwd = eic->fwd(); fwd->offset(130 * etm::cm);

    fwd->add("MPGD",      15 * etm::cm)->brick();//->highlight();
    fwd->marker();

    fwd->add("Fwd RICH", 150 * etm::cm)->trim(0.7, 1.0);//->highlight();
    for(unsigned nn=0; nn<2; nn++)
      fwd->add("TRD",     15 * etm::cm)->brick();//->highlight();

    fwd->add("TOF",       10 * etm::cm)->brick();
    fwd->add("EmCal",     40 * etm::cm);//->highlight();
    fwd->add("HCal",     120 * etm::cm);//->highlight();
  } 

  // Electron-going endcap;
  {
    auto bck = eic->bck(); bck->offset(130 * etm::cm);

    bck->add("MPGD",      15 * etm::cm)->brick();//->highlight();

    bck->add("Cherenkov", 40 * etm::cm);//->highlight();
    bck->marker();
    bck->add("TOF",        5 * etm::cm)->brick();
    bck->add("EmCal",     65 * etm::cm)->trim(1.0, 0.0);//->highlight();
    bck->add("HCal",     105 * etm::cm);//->highlight();
  }

  // Declare eta boundary configuration;
  {
    eic->vtx()->get("Si Tracker")->stretch(eic->bck()->get("TOF"));
    eic->vtx()->get("Si Tracker")->stretch(eic->fwd()->get("Fwd RICH"));

    eic->mid()->get("TPC")       ->stretch(eic->fwd()->get("MPGD"));
    eic->mid()->get("TPC")       ->stretch(eic->bck()->get("MPGD"));
    eic->mid()->get("HCal")      ->stretch(eic->bck()->get("HCal"));

    eic->bck()->get("EmCal")     ->stretch(eic->mid()->get("DIRC"));

    //eic->mid()->get("EmCal")     ->stretch(eic->bck()->get("HCal"),   -25 * etm::cm);
    //eic->mid()->get("Cryostat")  ->stretch(eic->bck()->get("HCal"),   -63 * etm::cm);
    eic->bck()->get("EmCal")     ->stretch(eic->mid()->get("DIRC"));//, 200 * etm::cm);
    eic->mid()->get("Cryostat")  ->stretch(eic->bck()->get("HCal"),    -63 * etm::cm);
    eic->mid()->get("Cryostat")  ->stretch(eic->fwd()->get("TRD", 0), -103 * etm::cm);

    eic->bck()->get("HCal")      ->stretch(eic->mid()->get("HCal"),    90 * etm::cm);
    eic->mid()->get("HCal")      ->stretch(eic->fwd()->get("HCal"));
    eic->fwd()->get("HCal")      ->stretch(eic->mid()->get("HCal"),   120 * etm::cm);
    eic->fwd()->get("TRD", 0)    ->stretch(eic->mid()->get("HCal"),    40 * etm::cm);
    eic->fwd()->get("TRD", 1)    ->stretch(eic->mid()->get("HCal"),    40 * etm::cm);
    eic->fwd()->get("TOF")       ->stretch(eic->mid()->get("HCal"),    40 * etm::cm);
    eic->fwd()->get("EmCal")     ->stretch(eic->mid()->get("HCal"),    40 * etm::cm);
  }

  // Beautify picture a little bit;
  eic->ApplyStandardTrimming();

  // Draw horizontal cross cut view; write the .root file out;
  eic->hdraw();
  //eic->write();
  //eic->Export("EIC-IR1-XX-v01e.stp");
} 
