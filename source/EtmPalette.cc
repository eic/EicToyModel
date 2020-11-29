
#include <TStyle.h>

#include <EtmPalette.h>

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

EtmPalette::EtmPalette() 
{
  // Help transparency to work; do not rely on a fact that 
  // OpenGL.CanvasPreferGL is set to 1 in $ROOTSYS/etc/system.rootrc ;
  gStyle->SetCanvasPreferGL(kTRUE);

  // Pre-defined "regular" detector colors; would be best to never change them;
  AddEntry("",           _GAP_COLOR_);
  AddEntry("TRD",        kGray);
  AddEntry("TPC",        kCyan);
  //AddEntry("TOF",        kBlack);
  //AddEntry("Preshower",  kOrange-3);
  AddEntry("TOF",        kOrange-3);
  AddEntry("Preshower",  kBlack);
  AddEntry("Si Tracker", kYellow);
  AddEntry("Cherenkov",  kAzure+6);
  //AddEntry("DIRC",       kAzure+7);
  AddEntry("Mu Tagger",  kViolet);
  //AddEntry("LM PID",     kViolet+1);
  AddEntry("MPGD",       kGreen+2);

  // Keep both for backward compatibility;
  AddEntry("HM RICH",    kMagenta+1);
  AddEntry("Fwd RICH",   kMagenta);

  AddEntry("EmCal",      kGreen); 

  AddEntry("sTGC",       kViolet+2);
  AddEntry("DIRC",       kViolet+1);

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
