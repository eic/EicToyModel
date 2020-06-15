
#ifndef _ETM_ORPHANS_
#define _ETM_ORPHANS_

namespace etm {
  // For now I need only convenient length units I guess;
  static const double   cm =   1.0;
  static const double   mm =   0.1;
  static const double    m = 100.0;
  static const double inch =  2.54;

  enum Stretch {kDent, kRecess, kWall};

  enum LineStyle {solid = 1, dashed = 7, dotted = 3, dashdotted = 8};
}

extern "C" {
  double Eta2Theta(double   eta);
  double Theta2Eta(double theta);

  // FIXME: do a better exception handing scheme;
  bool ConditionChecker(bool condition, const char *fmt, const char *message = 0);
}

#endif
