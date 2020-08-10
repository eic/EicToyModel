// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef _EIC_VST_SUBSYSTEM_
#define _EIC_VST_SUBSYSTEM_

#include <g4detectors/PHG4DetectorSubsystem.h>

class EicVstDetector;
class PHCompositeNode;
class PHG4Detector;
class PHG4SteppingAction;

#include <VstGeoParData.h>

/**
   * \brief Detector Subsystem module
   *
   * The detector is constructed and registered via EicVstDetector
   *
   *
   * \see EicVstDetector
   * \see EicVstSubsystem
   *
   */
class EicVstSubsystem : public PHG4DetectorSubsystem, public VstGeoParData
{
 public:
  //! constructor
  EicVstSubsystem(const std::string& name = "EicVst");

  //! destructor
  EicVstSubsystem() {};

  /*!
  creates relevant hit nodes that will be populated by the stepping action and stored in the output DST
  */
  int InitRunSubsystem(PHCompositeNode*) override;

  //! event processing
  /*!
  get all relevant nodes from top nodes (namely hit list)
  and pass that to the stepping action
  */
  int process_event(PHCompositeNode*) override;

  //! accessors (reimplemented)
  PHG4Detector* GetDetector() const override;
  EicVstDetector* GetVstDetector() const { return m_Detector; };

  PHG4SteppingAction* GetSteppingAction() const override { return m_SteppingAction; }

  //! Print info (from SubsysReco)
  void Print(const std::string& what = "ALL") const override;

 protected:
  // \brief Set default parameter values
  void SetDefaultParameters() override;

 private:
  //! detector construction
  /*! derives from PHG4Detector */
  EicVstDetector* m_Detector;

  //! particle tracking "stepping" action
  /*! derives from PHG4SteppingActions */
  PHG4SteppingAction* m_SteppingAction;
};

#endif 
