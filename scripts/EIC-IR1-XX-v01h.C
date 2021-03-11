
{
  auto eic = new EicToyModel();

  // Shift IP if needed; define canvas width; request eta=0 line in the drawing; set name;
  eic->ip(0.0 * etm::cm)->width(1200)->AddEtaLine(0.0)->SetName("EIC-IR1-XX-v01h");
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
      
    mid->add("Si Tracker",20 * etm::cm);
    //mid->gap(              8 * etm::cm);
    for(unsigned iq=0; iq<4; iq++) {
      //mid->gap(           10 * etm::cm);
      mid->add("MPGD",    10 * etm::cm);
      if (iq != 3) 
	mid->gap(        (28 * etm::cm)/3.);
    } //for iq
    mid->add("DIRC",      12 * etm::cm)->trim(1.0, 0.3);//->highlight();
    mid->add("EmCal",     40 * etm::cm);//->highlight();
    mid->add("Cryostat",  50 * etm::cm)->trim(1.0, 0.6);
    mid->add("HCal",     120 * etm::cm)->trim(0.0, 1.00);//->highlight();;
  }

  // Hadron-going endcap;
  {
    auto fwd = eic->fwd(); fwd->offset(100 * etm::cm);

    for(unsigned iq=0; iq<3; iq++) {
      fwd->gap(              5 * etm::cm);
      fwd->add("MPGD",      10 * etm::cm)->brick();//->highlight();
    } //for iq
    fwd->marker();

    fwd->add("Fwd RICH", 150 * etm::cm)->trim(1.0, 1.0);//->highlight();
    for(unsigned nn=0; nn<2; nn++)
      fwd->add("TRD",     15 * etm::cm)->brick();//->highlight();

    fwd->add("TOF",       10 * etm::cm)->brick();
    fwd->add("EmCal",     40 * etm::cm);//->highlight();
    fwd->add("HCal",     120 * etm::cm);//->highlight();
  } 

  // Electron-going endcap;
  {
    auto bck = eic->bck(); bck->offset(100 * etm::cm);

    for(unsigned iq=0; iq<3; iq++) {
      bck->gap(              5 * etm::cm);
      bck->add("MPGD",      10 * etm::cm)->brick();//->highlight();
    } //for iq

    bck->add("Cherenkov", 40 * etm::cm);//->highlight();
    bck->marker();
    bck->add("TOF",       10 * etm::cm)->brick();
    bck->add("EmCal",     60 * etm::cm)->trim(1.0, 0.0);//->highlight();
    bck->add("HCal",     105 * etm::cm);//->highlight();
  }

  // Declare eta boundary configuration;
  {
    eic->vtx()->get("Si Tracker")  ->stretch(eic->bck()->get("TOF"));
    eic->vtx()->get("Si Tracker")  ->stretch(eic->fwd()->get("Fwd RICH"));

    eic->mid()->get("Si Tracker")  ->stretch(eic->fwd()->get("MPGD"));
    eic->mid()->get("Si Tracker")  ->stretch(eic->bck()->get("MPGD"));
    eic->mid()->get  ("MPGD",    0)->stretch(eic->fwd()->get("MPGD",  0));
    eic->mid()->get  ("MPGD",    0)->stretch(eic->bck()->get("MPGD",  0));
    for(unsigned iq=0; iq<3; iq++) {
      eic->mid()->get("MPGD", iq+1)->stretch(eic->bck()->get("MPGD", iq),10 * etm::cm);
      eic->mid()->get("MPGD", iq+1)->stretch(eic->fwd()->get("MPGD", iq),10 * etm::cm);
    } //for iq
    //eic->fwd()->get  ("MPGD",    2)->stretch(eic->mid()->get("MPGD",  3));
    eic->mid()->get("HCal")        ->stretch(eic->bck()->get("HCal"));

    eic->bck()->get("EmCal")       ->stretch(eic->mid()->get("DIRC"));

    eic->mid()->get("EmCal")       ->stretch(eic->bck()->get("HCal"),   -25 * etm::cm);

    eic->bck()->get("HCal")        ->stretch(eic->mid()->get("HCal"),    90 * etm::cm);
    eic->mid()->get("HCal")        ->stretch(eic->fwd()->get("HCal"));
    eic->fwd()->get("HCal")        ->stretch(eic->mid()->get("HCal"),   120 * etm::cm);
    eic->fwd()->get("TRD", 0)      ->stretch(eic->mid()->get("HCal"),    10 * etm::cm);
    eic->fwd()->get("TRD", 1)      ->stretch(eic->mid()->get("HCal"),    20 * etm::cm);
    eic->fwd()->get("TOF")         ->stretch(eic->mid()->get("HCal"),    30 * etm::cm);
    eic->fwd()->get("EmCal")       ->stretch(eic->mid()->get("HCal"),    40 * etm::cm);
  }

  // Beautify picture a little bit;
  eic->ApplyStandardTrimming();

  // Draw horizontal cross cut view; write the .root file out;
  eic->hdraw();
  eic->write();
  //eic->Export("EIC-IR1-XX-v01h.stp");
} 
