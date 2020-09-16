
//
//  VST geometry creation and tracking;
//

#include <fun4all/Fun4AllServer.h>
#include <g4main/PHG4Reco.h>
#include <g4main/PHG4Detector.h>
#include <g4main/PHG4ParticleGenerator.h>
#include <g4main/PHG4TruthSubsystem.h>
#include <g4trackfastsim/PHG4TrackFastSim.h>
#include <phool/recoConsts.h>

#include <EicToyModelSubsystem.h>
#include <EicRootVstSubsystem.h>
#include <EtmOrphans.h>

#include <TrackFastSimEval.h>

R__LOAD_LIBRARY(libeicdetectors.so)
// FIXME: add to CMakeLists.txt;
R__LOAD_LIBRARY(libg4trackfastsim.so)

// Optionally may want to place the detectors into the respective EicToyModel integration volume bubbles;
//#define _USE_INTEGRATION_VOLUMES_

void Fun4All_G4_Tracking(int nEvents = 1000)
{
  // The fun4all Server;
  Fun4AllServer *se = Fun4AllServer::instance();

  // May want to fix the random seed for reproducibility;
  recoConsts::instance()->set_IntFlag("RANDOMSEED", 12345);

  // Particle Generator Setup;
  {
    auto gen = new PHG4ParticleGenerator();

    gen->set_name("pi+");           	  // geantino, pi-, pi+, mu-, mu+, e-., e+, proton, ... 
    gen->set_vtx(0,0,0);		  // Vertex generation range
    gen->set_mom_range(10., 10.);	  // Momentum generation range in GeV/c
    gen->set_eta_range(-.1,.1);		  // Detector coverage around theta ~ 90 degrees in this example
    gen->set_phi_range(0.,2.*TMath::Pi());
    se->registerSubsystem(gen); 
  }

  // Geant4 setup;
  PHG4Reco* g4Reco = new PHG4Reco();
  // Well, the GDML export does not work properly for these volumes;
  //g4Reco->save_DST_geometry(false);

  // BeAST magnetic field;
  g4Reco->set_field_map(string(getenv("CALIBRATIONROOT")) + string("/Field/Map/mfield.4col.dat"), PHFieldConfig::kFieldBeast);

  // EicToyModel integration volumes;
#ifdef _USE_INTEGRATION_VOLUMES_
  auto etm = new EicToyModelSubsystem("EicToyModel");
  etm->set_string_param("EtmInputRootFile", "../../build/eicroot.root");
  etm->SetActive(false);
  g4Reco->registerSubsystem(etm);
#endif

  // EicRoot media import; neither bound to EicToyModel nor to a particular EicRoot detector;
  EicGeoParData::ImportMediaFile("../../examples/eicroot/media.geo");

  // EicRoot vertex tracker; be aware: "VST" will also become a SuperDetector name;
  auto vst = new EicRootVstSubsystem("VST");
  {
    vst->SetGeometryType(EicGeoParData::NoStructure);
    vst->SetActive(true);
    // Otherwise will be placed into the world volume directly;
#ifdef _USE_INTEGRATION_VOLUMES_
    vst->set_string_param("MotherVolume", "TRACKER");
#endif

    // Barrel layers; hits belonging to these layers will be labeled internally
    // according to the sequence of these calls;
    {
      auto ibcell = new MapsMimosaAssembly();
  
      // Compose barrel layers; parameters are:
      //  - cell assembly type;
      //  - number of staves in this layer;
      //  - number of chips in a stave;
      //  - chip center installation radius;
      //  - additional stave slope around beam line direction; [degree];
      //  - layer rotation around beam axis "as a whole"; [degree];
      vst->AddBarrelLayer(ibcell, 1*3*12,  1*9, 1*3*23.4 * etm::mm, 12.0, 0.0);
      vst->AddBarrelLayer(ibcell, 2*3*12,  1*9, 2*3*23.4 * etm::mm, 12.0, 0.0);
      vst->AddBarrelLayer(ibcell, 3*3*12,  1*9, 3*3*23.4 * etm::mm, 12.0, 0.0);
      vst->AddBarrelLayer(ibcell, 4*3*12,  1*9, 4*3*23.4 * etm::mm, 12.0, 0.0);
    }

    g4Reco->registerSubsystem(vst);
  }

  // Truth information;
  g4Reco->registerSubsystem(new PHG4TruthSubsystem());
  se->registerSubsystem(g4Reco);

  // Ideal track finder and Kalman filter;
  {
    auto kalman = new PHG4TrackFastSim("PHG4TrackFastSim");

    kalman->set_use_vertex_in_fitting(false);

    kalman->add_phg4hits(vst->GetG4HitName(),		// const std::string& phg4hitsNames
			 PHG4TrackFastSim::Cylinder,	// const DETECTOR_TYPE phg4dettype
			 999.,				// radial-resolution [cm] (this number is not used in cylindrical geometry)
			 5.8e-4,			// azimuthal (arc-length) resolution [cm]
			 5.8e-4,			// longitudinal (z) resolution [cm]
			 1,				// efficiency (fraction)
			 0);				// hit noise

    // 20e-4/sqrt(12) cm = 5.8e-4 cm, to emulate 20x20 um pixels;
    se->registerSubsystem(kalman);
  }

  // User analysis code: just a single dp/p histogram;
  se->registerSubsystem(new TrackFastSimEval());

  // Run it all, eventually;
  se->run(nEvents);
  se->End();
  delete se;
  gSystem->Exit(0);
} // Fun4All_G4_Sandbox()
