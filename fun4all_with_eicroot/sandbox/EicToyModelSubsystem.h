// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef _EIC_TOY_MODEL_SUBSYSTEM_
#define _EIC_TOY_MODEL_SUBSYSTEM_

#include <g4detectors/PHG4DetectorSubsystem.h>

class EicToyModelDetector;
class PHCompositeNode;
class PHG4Detector;

/**
   * \brief Detector Subsystem module
   *
   * The detector is constructed and registered via EicToyModelDetector
   *
   *
   * \see EicToyModelDetector
   * \see EicToyModelSubsystem
   *
   */
class EicToyModelSubsystem : public PHG4DetectorSubsystem
{
 public:
  //! constructor
  EicToyModelSubsystem(const std::string& name = "EicToyModel");

  //! destructor
  ~EicToyModelSubsystem() {};

  /*!
  creates relevant hit nodes that will be populated by the stepping action and stored in the output DST
  */
  int InitRunSubsystem(PHCompositeNode*) override;

  //! accessors (reimplemented)
  PHG4Detector* GetDetector() const override;

  //! Print info (from SubsysReco)
  void Print(const std::string& what = "ALL") const override;

 protected:
  // \brief Set default parameter values
  void SetDefaultParameters() override;

 private:
  //! detector construction
  /*! derives from PHG4Detector */
  EicToyModelDetector* m_Detector;
};

#endif
