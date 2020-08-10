
#include <fun4all/Fun4AllServer.h>
#include <g4main/PHG4Reco.h>
#include <g4main/PHG4Detector.h>

#include <EicToyModelSubsystem.h>
#include <EicVstSubsystem.h>
#include <EtmOrphans.h>

R__LOAD_LIBRARY(libeicdetectors.so)

void Fun4All_G4_Sandbox( void )
{
  // Make the Server;
  Fun4AllServer *se = Fun4AllServer::instance();

  // Geant4 setup;
  PHG4Reco* g4 = new PHG4Reco();
  // Well, the GDML export does not work;
  g4->save_DST_geometry(false);

  // EicToyModel integration volumes;
  auto etm = new EicToyModelSubsystem("EicToyModel");
  etm->set_string_param("EtmInputRootFile", "/scratch/EicToyModel/build/eicroot.root");
  etm->SetActive(false);
  //beast->SuperDetector("MAGNET");
  g4->registerSubsystem(etm);

  // EicRoot media import; neither bound to EicToyModel nor to a particular EicRoot detector;
  EicGeoParData::ImportMediaFile("/scratch/EicToyModel/examples/eicroot/media.geo");

  // EicRoot vertex tracker;
  auto vst = new EicVstSubsystem("EicVst");
  vst->SetActive(true);
  vst->set_string_param("MotherVolume", "TRACKER");
  {
    auto ibcell = new MapsMimosaAssembly();
  
    // Compose barrel layers; parameters are:
    //  - cell assembly type;
    //  - number of staves in this layer;
    //  - number of chips in a stave;
    //  - chip center installation radius;
    //  - additional stave slope around beam line direction; [degree];
    //  - layer rotation around beam axis "as a whole"; [degree];
    vst->AddBarrelLayer(ibcell, 3*12,  9, 3*23.4 * etm::mm, 12.0, 0.0);
  }
  g4->registerSubsystem(vst);

  se->registerSubsystem(g4);

  g4->InitRun(se->topNode());
  g4->ApplyDisplayAction();
  g4->StartGui();
} // Fun4All_G4_Sandbox()
