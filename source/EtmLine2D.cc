#include <assert.h>
#include <math.h>

#include <EtmLine2D.h>

// -------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------

EtmLine2D::EtmLine2D(const TVector2 &x, const TVector2 &v): mX(x), mV(v.Unit()) 
{
  CalculateNormal();
} // EtmLine2D::EtmLine2D()

// -------------------------------------------------------------------------------------------

EtmLine2D::EtmLine2D(const TVector2 &x, double alfa): mX(x) {
  mV = TVector2(cos(alfa), sin(alfa));

  CalculateNormal();
} // EtmLine2D::EtmLine2D()

// -------------------------------------------------------------------------------------------

void EtmLine2D::CalculateNormal( void )
{
  mN = mV.Rotate(M_PI/2);
} // EtmLine2D::CalculateNormal()

// -------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------

TVector2 EtmLine2D::Cross(const EtmLine2D &line) const
{
  // Efficiency is least of concerns for this code;
  const TVector2 &x1 = mX, &v1 = mV, &x2 = line.mX, &v2 = line.mV;
  double x1v2 = x1*v2, x1v1 = x1*v1, v1v2 = v1*v2, x2v2 = x2*v2, x2v1 = x2*v1;

  // FIXME: do it better later;
  assert(v1v2*v1v2 != 1.0);
  double t1 = (x2v2*v1v2 - x2v1 + x1v1 - x1v2*v1v2)/(v1v2*v1v2 - 1.0);

  return x1 + t1*v1;
} // EtmLine2D::Cross()

// -------------------------------------------------------------------------------------------

double EtmLine2D::Distance(const TVector2 &point) const
{
  return (point - mX)*mN;
} // EtmLine2D::Distance()

// -------------------------------------------------------------------------------------------

bool EtmLine2D::IsParallel(const EtmLine2D &line) const
{
  // FIXME: do it better later; for now I'm only interested in EtmLine2D::Cross() to work;
  double v1v2 = mV*line.mV;

  return (v1v2*v1v2 == 1.0);
} // EtmLine2D::IsParallel()

// -------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------

ClassImp(EtmLine2D)
