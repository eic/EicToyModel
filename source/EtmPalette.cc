
#include <EtmPalette.h>

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

EtmPalette::EtmPalette() 
{
  // Pre-defined "regular" detector colors; would be best to never change them;
  AddEntry("",           _GAP_COLOR_);
  AddEntry("TRD",        kGray);
  AddEntry("TPC",        kCyan);
  AddEntry("TOF",        kBlack);
  AddEntry("Preshower",  kOrange-3);
  AddEntry("Si Tracker", kYellow);
  AddEntry("Cherenkov",  kAzure+6);
  AddEntry("Mu Tagger",  kViolet);
  AddEntry("LM PID",     kViolet+1);
  AddEntry("MPGD",       kGreen+2);
  AddEntry("HM RICH",    kMagenta);
  AddEntry("EmCal",      kGreen); 

  // Will not be visible anyway;
  AddEntry(_MARKER_,     kPink-8);

  AddEntry("PID",        kPink-9);
  AddEntry("TRACKER",    kCyan-10);


  // This stuff has to do with the "magnet element" look -> don't touch;
  //AddEntry("Cryostat",   kYellow-10);//White);
  //AddEntry("Cryostat",   kCyan+2);//White);
  AddEntry("Cryostat",   kYellow-8);
  AddEntry("M.Coil +",   kRed);
  AddEntry("M.Coil -",   kAzure+4);
  AddEntry("HCal",       kBlue+1);
} // EtmPalette::EtmPalette() 

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

bool EtmPalette::AddEntry(const char *tag, int color) 
{
  if (mColors.find(tag) != mColors.end() || mTags.find(color) != mTags.end()) {
    printf("Either '%s' tag or '%d' color exist already!\n", tag, color);
    return false;
  } //if
  
  mColors[tag] = color;
  mTags[color] = tag;
  
  return true;
} // EtmPalette::AddEntry()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

ClassImp(EtmPalette)
ClassImp(EtmPaletteEntry)
