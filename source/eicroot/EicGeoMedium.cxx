
//
// A renamed excerpt of FairGeoMedium.cxx;
//

#include <iostream>
#include <cmath>
#include "stdlib.h"

using std::cout;
using std::log;
using std::pow;

#include "EicGeoMedium.h"

// ---------------------------------------------------------------------------------------

EicGeoMedium::EicGeoMedium(const char* name): 
  TNamed(name,""),
  //@@@medId(0),
  autoflag(1),
  nComponents(0),
  weightFac(0),
  ca(NULL),
  cz(NULL),
  cw(NULL),
  density(0),
  //@@@radLen(0),
  sensFlag(0),
  fldFlag(0),
  fld(0),
  epsil(0),
  madfld(-1),
  maxstep(-1),
  maxde(-1),
  minstep(-1)//,
  //npckov(0),
  //    ppckov(NULL),
  //  absco(NULL),
  //  effic(NULL),
  //  rindex(NULL)
{
  // Constructor for a medium with name and index id
  //  SetName(name);
} // EicGeoMedium::EicGeoMedium()

// ---------------------------------------------------------------------------------------

EicGeoMedium::~EicGeoMedium()
{
  // Destructor
  if (nComponents>0) {
    delete [] ca;
    ca=0;
    delete [] cz;
    cz=0;
    delete [] cw;
    cw=0;
    nComponents=0;
  }
#if _OLD_
  if (npckov>0) {
    delete [] ppckov;
    ppckov=0;
    delete [] absco;
    absco=0;
    delete [] effic;
    effic=0;
    delete [] rindex;
    rindex=0;
    npckov=0;
  }
#endif
} // EicGeoMedium::~EicGeoMedium()

// ---------------------------------------------------------------------------------------

void EicGeoMedium::setNComponents(Int_t n)
{
  // Sets the number of components in the material
  if (n==0) { return; }
  Int_t k=abs(n);
  if (nComponents!=0 && k!=nComponents) {
    delete [] ca;
    delete [] cz;
    delete [] cw;
    nComponents=0;
  }
  if (nComponents==0) {
    nComponents=k;
    ca=new Double_t[k];
    cz=new Double_t[k];
    cw=new Double_t[k];
  }
  weightFac=(Int_t)(n/nComponents);
} // EicGeoMedium::setNComponents()

// ---------------------------------------------------------------------------------------

Bool_t EicGeoMedium::setComponent (Int_t i,Double_t a,Double_t z,Double_t weight)
{
  // Defines the ith material component
  if (i<0||i>=nComponents) {
    Error("setNComponents","Wrong index");
    return kFALSE;
  }
  ca[i]=a;
  cz[i]=z;
  cw[i]=weight;
  return kTRUE;
} // EicGeoMedium::setComponent()

// ---------------------------------------------------------------------------------------

void EicGeoMedium::getComponent(Int_t i,Double_t* p)
{
  // Returns the ith material component
  if (i>=0&&i<nComponents) {
    p[0]=ca[i];
    p[1]=cz[i];
    p[2]=cw[i];
//   cout << " -I p: " << p[0] << p[1] << p[2] << endl;
  } else { p[0]=p[1]=p[2]=0.; }
} // EicGeoMedium::getComponent()

// ---------------------------------------------------------------------------------------

void EicGeoMedium::setMediumPar(Int_t sensitivityFlag,Int_t fieldFlag,
                                 Double_t maxField,Double_t precision,Double_t maxDeviation,
                                 Double_t maxStep,Double_t maxDE,Double_t minStep )
{
  // Sets the medium parameters
  sensFlag=sensitivityFlag;
  fldFlag=fieldFlag;
  fld=maxField;
  epsil=precision;
  madfld=maxDeviation;
  maxstep=maxStep;
  maxde=maxDE;
  minstep=minStep;
} // EicGeoMedium::setMediumPar()

// ---------------------------------------------------------------------------------------

void EicGeoMedium::getMediumPar(Double_t* params)
{
  // Returns the medium parameters
  params[0]=sensFlag;
  params[1]=fldFlag;
  params[2]=fld;
  params[3]=madfld;
  params[4]=maxstep;
  params[5]=maxde;
  params[6]=epsil;
  params[7]=minstep;
  params[8]=0.;
  params[9]=0.;
} // EicGeoMedium::getMediumPar()

// ---------------------------------------------------------------------------------------

void EicGeoMedium::read(std::fstream& fin, Int_t aflag )
{
  // Reads the parameters from file
  autoflag=aflag;
  Int_t n;
  fin>>n;
  setNComponents(n);
  for(Int_t ik=0; ik<nComponents; ik++) {
    fin>>ca[ik];
  }
  for(Int_t i=0; i<nComponents; i++) {
    fin>>cz[i];
  }
  fin>>density;
  if (nComponents==1) {
    cw[0]=1.;
    //@@@calcRadiationLength();
  } else {
    for(Int_t i=0; i<nComponents; i++) {
      fin>>cw[i];
    }
  }
  fin>>sensFlag>>fldFlag>>fld>>epsil ;
  if (autoflag<1) { fin>>madfld>>maxstep>>maxde>>minstep; }
  else {
    //to use this feature one has to set TGeant3::SetAUTO(0), thus if the media does not
    // defined these values one can force Geant3 to calculate them by given them a value
    // of -1
    madfld=-1;
    maxstep=-1;
    maxde=-1;
    minstep=-1;
  }
  fin>>n;
  //setNpckov(n);
  if (n>0) {
    double ppckov, absco, effic, rindex;
    for(Int_t i=0; i<n; i++) {
      //@@@fin>>ppckov[i]>>absco[i]>>effic[i]>>rindex[i];
      fin>>ppckov>>absco>>effic>>rindex;
    }
  }

} // EicGeoMedium::read()

// ---------------------------------------------------------------------------------------

void EicGeoMedium::print()
{
  // Prints the medium definition
  const char* bl="  ";
  cout<<GetName()<<'\n'<<nComponents* weightFac<<bl;
  for(Int_t ii=0; ii<nComponents; ii++) { cout<<ca[ii]<<bl ;}
  for(Int_t j=0; j<nComponents; j++) { cout<<cz[j]<<bl ;}
  cout<<density<<bl;
  if (nComponents<2) { /*@@@cout<<radLen;*/ }
  else for(Int_t iik=0; iik<nComponents; iik++) { cout<<cw[iik]<<bl ;}
  cout<<'\n'<<sensFlag<<bl<<fldFlag<<bl<<fld<<bl<<epsil<<'\n';
  if (autoflag<1) {
    cout<<madfld<<bl<<maxstep<<bl<<maxde<<bl<<minstep<<'\n';
  }
#if _OLD_
  cout<<npckov<<'\n';
  if (npckov>0) {
    for(Int_t i=0; i<npckov; i++) {
      cout<<ppckov[i]<< bl<<absco[i]<<bl<<effic[i]<<bl<<rindex[i]<<'\n';
    }
  }
#endif
  cout<<'\n';
} // EicGeoMedium::print()

// ---------------------------------------------------------------------------------------

ClassImp(EicGeoMedium)
