{
  auto eic = new EicToyModel();

  // Shift IP if needed; define canvas width; request eta=0 line in the drawing; set name;
  eic->ip(-50.0)->width(1500)->AddEtaLine(0.0)->SetName("calorimetry");
  // Define acceptance ranges and the vacuum chamber design;
  eic->acceptance(-4.2, -1.0, 1.2, 4.2);
  //++eic->DefineVacuumChamber(new vc2020_03_20());

  // Barrel;
  {
    // etm::cm can be safely omitted everywhere, it is equal 1.0 and will never change;
    auto mid = eic->mid(); mid->offset( 100 * etm::cm);
      
    mid->add("EmCal",     30 * etm::cm);
    mid->add("Cryostat",  40 * etm::cm);
    mid->add("HCal",      80 * etm::cm);
    mid->add("HCal",      40 * etm::cm)->trim(0.0, 0.5);
  }

  // Hadron-going endcap;
  {
    auto fwd = eic->fwd(); fwd->offset(250 * etm::cm);

    fwd->add("EmCal",     35 * etm::cm);
    fwd->add("HCal",     105 * etm::cm)->trim(1.0, 0.0);
  } 

  // Electron-going endcap;
  {
    auto bck = eic->bck(); bck->offset(200 * etm::cm);

    bck->add("EmCal",     35 * etm::cm)->trim(0.0, 1.0);
    bck->add("HCal",     105 * etm::cm)->trim(0.0, 1.0);
  }

  // Declare eta boundary configuration;
  {
    eic->bck()->get("HCal")   ->stretch(eic->mid()->get("HCal", 1), 40 * etm::cm);
    eic->bck()->get("EmCal")  ->stretch(eic->mid()->get("Cryostat"));

    eic->mid()->get("EmCal")  ->stretch(eic->bck()->get("EmCal"),   20 * etm::cm);
    eic->mid()->get("EmCal")  ->stretch(eic->fwd()->get("HCal"));

    eic->mid()->get("HCal", 0)->stretch(eic->bck()->get("HCal"));
    eic->mid()->get("HCal", 0)->stretch(eic->fwd()->get("HCal"),    50 * etm::cm);
    eic->mid()->get("HCal", 1)->stretch(eic->bck()->get("HCal"),    50 * etm::cm);
  }

  // Beautify picture a little bit;
  eic->ApplyStandardTrimming();

  // Draw vertical cross cut view; write the .root file out;
  eic->vdraw();
  eic->write();
} 
