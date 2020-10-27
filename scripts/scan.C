
void scan(const char *fin, const char *fout = 0)
{
  // Import the EicToyModel model singleton class instance;
  auto eic = EicToyModel::Import(fin);
  // Build the vacuum chamber TGeo geometry;
  eic->BuildVacuumChamber();
  
  // Request azimuthal scan in the -4.5 < eta < -3.5 range (e-endcap);
  //auto mfscan = new EtmAzimuthalScan(-4.5, -3.5);
  auto mfscan = new EtmAzimuthalScan(3.5, 4.5);

  // Example: constant 3T field;
  mfscan->SetBfield(3.0);
  // Example: location of the "mfield.4col.dat" BeastMagneticField library data file;
  //mfscan->SetBfield("<path>/mfield.4col.dat");

  // Other options: vertex smearing along the beam line (default: 0), 
  // statistics (default: 100; reset to 1, if vertex smearing is zero);
  mfscan->SetVertexSigma(8 * etm::cm)->SetStat(1000);

  // Perform scan;
  mfscan->DoIt(fout);

  exit(0);
} // scan()
