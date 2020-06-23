
#include <vector>

#include <TString.h>
#include <TVector2.h>
#include <TGeoManager.h>

#ifndef _EIC_VACUUM_CHAMBER_
#define _EIC_VACUUM_CHAMBER_

class G4VSolid;
class EicToyModel;

#define _ACCELERATOR_VACUUM_ ("AcceleratorVacuum")
#define _UNIVERSE_VACUUM_    ("UhiverseVacuum")

class EtmVacuumChamber: public TObject {
 public:
  EtmVacuumChamber( void );
  ~EtmVacuumChamber() {};

  void DrawMe( void );// const;

  void SetHadronBeamPipeOpening    (double value)   { mHadronBeamPipeOpening = value; };
  double GetHadronBeamPipeOpening    ( void ) const { return mHadronBeamPipeOpening; };

  double GetRadialSize(double z, double phi);// const;

  TGeoManager *GetWorld( void ) const { return mTGeoModel; };
  void CheckGeometry(bool force = false);
  void Export(const char *fname);

  // The idea behind all this is to allow dynamic creation of the vacuum chamber volumes;
  // if the respective inherited class is not flexible enough to make use of the 
  // EicToyModel::mCrossingAngle, certain operations will be prohibited;
  virtual bool ConfigurableCrossingAngle( void ) const                    = 0;
  virtual double FixedCrossingAngle( void )      const { return 0.0; };
  bool CrossingAngleResetPossible(double value)  const;

  G4VSolid *CutThisSolid(G4VSolid *solid/*, const std::vector<TVector2> &polygon*/);

  void StoreGDMLdump( void );

 protected:
  virtual void CreateGeometry( void )                                     = 0;

  // It looks like there is no need to serialize the TGeoManager (?); the model can be 
  // re-created upon EicToyModel instance, and the model is dumped as a separate object 
  // in EtmToyModel::write() anyway;
  TGeoManager *mTGeoModel; //!

  // It looks like this parameter will always be there (?);
  double mHadronBeamPipeOpening;

  double mActualCrossingAngle; //!

  //std::vector<std::pair<TGeoShape*, TGeoMatrix*> > mCuttingSolids; //!

 private: 
  void CreateWorld( void ); 
  void CreateMedium(const char *name, double A, double Z, double density);

  bool mStandaloneMode; //!

  //Geant4GM::Factory *g4Factory;
  // FIXME: void* here is not good, but suffices; Geant4GM::Factory*?;
  void *g4Factory; //!

  //TString mGDMLdump;

  ClassDef(EtmVacuumChamber, 1)
};

#if 0
class XString: public TObject, public TString  {
public:
  XString() {};
  ~XString() {};

  ClassDef(XString, 1)
};
#endif
#endif
