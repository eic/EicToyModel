
#include <math.h>

#include <TString.h>

#include <EtmOrphans.h>

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

//
// This looks a bit stupid, but try-catch dos not improve things much; want to 
// keep going in the interactive mode rather than exit;
//

bool ConditionChecker(bool condition, const char *fmt, const char *message)
{
  if (!condition && fmt) {
    TString wrapperfmt; wrapperfmt.Form("\n\n   !!! %s !!!\n\n\n", fmt); 
    message ? printf(wrapperfmt.Data(), message) : printf(wrapperfmt.Data());
  } //if

  return condition;
} // EicDetector::ConditionChecker()

// ---------------------------------------------------------------------------------------

double Eta2Theta(double eta) 
{ 
  return 2*atan(exp(-eta));  
} // Eta2Theta()

// ---------------------------------------------------------------------------------------

double Theta2Eta(double theta) 
{ 
  return -log(tan(theta/2)); 
} // Theta2Eta()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
