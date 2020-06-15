
#include <TVector2.h>

#ifndef _ETM_LINE_2D_
#define _ETM_LINE_2D_

class EtmLine2D: public TObject {
public:
  EtmLine2D() {};
  EtmLine2D(const TVector2 &x, const TVector2 &v); 
  EtmLine2D(const TVector2 &x, double alfa); 
  ~EtmLine2D() {};

  TVector2 Cross (const EtmLine2D &line) const;
  double Distance(const TVector2 &point) const;
  bool IsParallel(const EtmLine2D &line) const;

  void Shift(const TVector2 &x) { mX += x; };

  TVector2 X( void ) const { return mX; };
  TVector2 V( void ) const { return mV; };
  TVector2 N( void ) const { return mN; };

private:
  void CalculateNormal( void );

  TVector2 mX, mV, mN;

  ClassDef(EtmLine2D, 1)
};

#endif
