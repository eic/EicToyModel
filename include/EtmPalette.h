
#include <map>

#include <TObject.h>
#include <TString.h>

#ifndef _ETM_PALETTE_
#define _ETM_PALETTE_

#include <EtmOrphans.h>

// NB: kWhite=0, so I seemingly need two distinct GetColor() return values:
// -1 (does not exist), and some odd number (gap);
#define _GAP_COLOR_ (-12345678)

// "MARKER" tag is hardcoded;
#define _MARKER_ ("MARKER")

class EtmPaletteEntry: public TObject {
 public:
 EtmPaletteEntry(int fillcolor = 0, int linecolor = 1):
  mFillColor(fillcolor), mLineColor(linecolor), mTextColor(kBlack), mLineStyle(etm::solid) {};
  ~EtmPaletteEntry() {};

  // Line color, fill color and line style is the same for all polygons;
  int mFillColor, mLineColor, mTextColor;
  etm::LineStyle mLineStyle;

  ClassDef(EtmPaletteEntry, 1)
};

class EtmPalette: public TObject {
 public:
  EtmPalette();
  ~EtmPalette() {};

  bool AddEntry(const char *tag, int color);

  int GetColor(const char *tag) const { return (Exists(tag)   ? mColors.at(tag)        : -1); }; 
  const char *GetTag(int color) const { return (Exists(color) ? mTags.at(color).Data() :  0); }; 

 private:
  bool Exists(const char *tag)  const { return mColors.find(tag) != mColors.end(); };
  bool Exists(int color)        const { return mTags.find(color) != mTags.end(); };

  // There is no C++ STL container with unique key and value; create both;
  std::map<TString, int> mColors;
  std::map<int, TString> mTags;

  ClassDef(EtmPalette, 1)
};

#endif
