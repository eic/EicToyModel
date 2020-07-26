
#include <TObject.h>
#include <TString.h>

#ifndef _EIC_POD_
#define _EIC_POD_

class EicPOD: public TObject {
 public:
  EicPOD() {};
  ~EicPOD() {};

  // double, Double_t, TString;
  int SetDoubleVariable (const char *vname, double      value)  const; 
  double GetDoubleVariable(const char *vname)                   const;
  int SetTStringVariable(const char *vname, const char *value)  const; 
  const TString *GetTStringVariable(const char *vname)          const;  

 private:
  char *GetVariableOffset(const char *vname, const char *vtype) const;

  ClassDef(EicPOD, 1)
};

#endif
