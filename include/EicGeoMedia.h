
//
// A renamed exerpt of FairGeoMedia.h;
//

#ifndef _EIC_GEO_MEDIA_
#define _EIC_GEO_MEDIA_

#include "TNamed.h"
#include "TString.h"

class EicGeoMedium;
class TList;

class EicGeoMedia : public TNamed
{
 private:
  TList* media;           /** list of media */
  Int_t nMed;  // number of defined media

 public:
  EicGeoMedia();
  ~EicGeoMedia();
  
  void addMedium(EicGeoMedium* m);
  EicGeoMedium* getMedium(const char*);
  TList* getListOfMedia() {return media;}
  void read(std::fstream&);
  void print();
  
  Int_t createMedium(EicGeoMedium*);
  
  ClassDef(EicGeoMedia,2)
};

#endif 
