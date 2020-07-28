
//
// A renamed exerpt of FairGeoMedium.h;
//

#include <fstream>

#ifndef _EIC_GEO_MEDIUM_
#define _EIC_GEO_MEDIUM_

#include "TNamed.h"

class EicGeoMedium  : public TNamed
{
 private:
  Int_t     autoflag;    /** Flag for default values of madfld .. minstep*/
  Int_t     nComponents; /** Number of material components */
  Int_t     weightFac;   /** Factor for weights (1: relative w., -1: w. by number of atoms)*/
  Double_t* ca;         //[nComponents] /** Atomic weights for the components*/
  Double_t* cz;         //[nComponents] /** Atomic numbers for the components*/
  Double_t* cw;         //[nComponents] /** Weights of the components in a mixture*/
  Double_t  density;     /** Density in g cm(**-3)*/
  Int_t     sensFlag;    /** Sensitivity flag*/
  Int_t     fldFlag;     /** Field flag*/
  Double_t  fld;         /** Maximum field value in kilogauss*/
  Double_t  epsil;       /** Boundary crossing precision*/
  Double_t  madfld;      /** Maximum angular deviation due to field*/
  Double_t  maxstep;     /** Maximum step permitted*/
  Double_t  maxde;       /** Maximum fractional energy loss*/
  Double_t  minstep;     /** Minimum value for step*/

 public:
  EicGeoMedium(const char* name="");
  ~EicGeoMedium();
  
  Int_t getAutoFlag() {return autoflag;}
  void setNComponents(Int_t);
  Int_t getNComponents() {return nComponents;}
  Bool_t setComponent(Int_t,Double_t,Double_t,Double_t w=1.);
  void getComponent(Int_t,Double_t*);
  Int_t getWeightFac() {return weightFac;}
  Double_t getDensity() {return density;}
  void setMediumPar(Int_t,Int_t,Double_t,Double_t,Double_t maxDeviation=-1.,
		    Double_t maxStep=-1.,Double_t maxDE=-1.,
		    Double_t minStepDouble_t=-1.);
  void getMediumPar(Double_t*);
  void read(std::fstream&, Int_t autoflag );
  void print();
  
  ClassDef(EicGeoMedium,2) 
};

#endif 
