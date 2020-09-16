
//
// A renamed excerpt of FairGeoMedia.cxx;
//

#include <iostream>

using std::cout;
using std::endl;

#include "TGeoMedium.h"
#include "TGeoMaterial.h"
#include "TList.h"

#include "EicGeoMedia.h"
#include "EicGeoMedium.h"

// ---------------------------------------------------------------------------------------

EicGeoMedia::EicGeoMedia(): 
  TNamed(),
  nMed(0),
  media(new TList())
{
} // EicGeoMedia::EicGeoMedia()

// ---------------------------------------------------------------------------------------

EicGeoMedia::~EicGeoMedia()
{
  if (media) {
    media->Delete();
    delete media;
    media=0;
  }
} // EicGeoMedia::~EicGeoMedia()

// ---------------------------------------------------------------------------------------

EicGeoMedium* EicGeoMedia::getMedium(const char* mediumName)
{
  // Returns the medium with name mediumName
  return (EicGeoMedium*)media->FindObject(mediumName);
} // EicGeoMedia::getMedium()

// ---------------------------------------------------------------------------------------

void EicGeoMedia::addMedium(EicGeoMedium* m)
{
  // Adds a medium to the list of media
  media->Add(m);
} // EicGeoMedia::addMedium()

// ---------------------------------------------------------------------------------------

void EicGeoMedia::print()
{
  TListIter iter(media);
  EicGeoMedium* medium;
  Int_t i=0;
  while((medium=(EicGeoMedium*)iter.Next())) {
    if (medium->getAutoFlag()!=0) {
      medium->print();
      i++;
    }
  }
  if (i!=media->GetSize()) {
    iter.Reset();
    cout<<"//----------------------------------------------\n";
    cout<<"AUTONULL\n";
    cout<<"//----------------------------------------------\n";
    while((medium=(EicGeoMedium*)iter.Next())) {
      if (medium->getAutoFlag()==0) { medium->print(); }
    }
  }
} // EicGeoMedia::print()

// ---------------------------------------------------------------------------------------

void EicGeoMedia::read(std::fstream& fin)
{
  // Reads the media from file
  cout<<"-I- EicGeoMedia  Read media"<<endl;
  const Int_t maxBuf=256;
  char buf[maxBuf];
  Int_t autoflag=1;
  while(!fin.eof()) {
    fin>>buf;
    if (buf[0]=='\0' || buf[0]=='/') { fin.getline(buf,maxBuf); }
    else if (fin.eof()) { break; }
    else {
      TString eleName(buf);
      if (eleName.CompareTo("AUTONULL")!=0) {
        EicGeoMedium* medium=new EicGeoMedium(eleName);
        medium->read(fin,autoflag);
        media->Add(medium);
      } else { autoflag=0; }
    }
  }
} // EicGeoMedia::read()

// ---------------------------------------------------------------------------------------

Int_t EicGeoMedia::createMedium(EicGeoMedium* med)
{
  // Creates the medium
  //@@@if (!geoManager&&!med) { return 0; }

  Int_t nComp=med->getNComponents();
  Int_t weightFac=med->getWeightFac();
  TGeoMaterial* material=0;
  Double_t p[3];
  if (nComp==1) {
    med->getComponent(0,p);
    material=new TGeoMaterial(med->GetName(),p[0],p[1],med->getDensity());//,
    //med->getRadiationLength());
    // Interaction length not defined!!!!!!
  } else {
    material=new TGeoMixture(med->GetName(),nComp,med->getDensity());
    Double_t sumWeights=0.;
    if (weightFac<0) {
      for(Int_t i=0; i<nComp; i++) {
        med->getComponent(i,p);
        sumWeights+=p[0]*p[2];
      }
    }
    for(Int_t i=0; i<nComp; i++) {
      med->getComponent(i,p);
      if (weightFac>0) {
        ((TGeoMixture*)material)->DefineElement(i,p[0],p[1],p[2]);
      } else {
        ((TGeoMixture*)material)->DefineElement(i,p[0],p[1],p[0]*p[2]/sumWeights);
      }
    }
  }
  nMed++;
  //@@@med->setMediumIndex(nMed);
  Double_t mp[10];
  med->getMediumPar(mp);
  TGeoMedium* medium=new TGeoMedium(med->GetName(),nMed,material,mp);
  if (medium) { return nMed; }
  else { return 0; }
} // EicGeoMedia::createMedium()

// ---------------------------------------------------------------------------------------

ClassImp(EicGeoMedia)
