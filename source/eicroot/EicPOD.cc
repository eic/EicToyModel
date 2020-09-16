
#include <TROOT.h>
#include <TClass.h>
#include <TRealData.h>
#include <TDataMember.h>

#include <EicPOD.h>

// ---------------------------------------------------------------------------------------

//
// Follow the logic of EicGeoParData::Print(); do not care about performance here;
//

int EicPOD::SetDoubleVariable(const char *vname, double value) const
{
  // Well, assume it is either 'double' or 'Double_t';
  auto addr = GetVariableOffset(vname, "Double_t");
  if (!addr) {
    addr = GetVariableOffset(vname, "double");
    
    if (!addr) return -1;
  } //if

  *(Double_t*)addr = value;

  return 0;
} // EicPOD::SetDoubleVariable()

double EicPOD::GetDoubleVariable(const char *vname) const
{
  auto addr = GetVariableOffset(vname, "Double_t");
  if (!addr) addr = GetVariableOffset(vname, "double");
    
  return (addr ? *(Double_t*)addr : 0.0);
} // EicPOD::GetDoubleVariable()

// ---------------------------------------------------------------------------------------

int EicPOD::SetTStringVariable(const char *vname, const char *value) const
{
  auto addr = GetVariableOffset(vname, "TString");
  if (!addr) return -1;

  *(TString*)addr = value;

  return 0;
} // EicPOD::SetTStringVariable() 

const TString *EicPOD::GetTStringVariable(const char *vname) const 
{
  auto addr = GetVariableOffset(vname, "TString");
  
  return (addr ? (TString*)addr : 0);
} // EicPOD::GetTStringVariable()

// ---------------------------------------------------------------------------------------

char *EicPOD::GetVariableOffset(const char *vname, const char *vtype) const
{
  TList *dataList = gROOT->GetClass(ClassName())->GetListOfRealData();
  TIter next(dataList);

  TRealData *data;
  while ((data=(TRealData*)next())) {
    TDataMember *member = data->GetDataMember();

    if (!strcmp(data->GetName(), vname) && 
	!strcmp(member->GetFullTypeName(), vtype))
      return (char*)this + member->GetOffset();
  } //while  

  return 0;
} // EicPOD::GetVariableOffset()

// ---------------------------------------------------------------------------------------

ClassImp(EicPOD)
