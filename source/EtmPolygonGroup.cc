
#include <TPolyLine.h>

#include <EicToyModel.h>
#include <EtmPolygonGroup.h>

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void EtmPolygonGroup::AddPolygon(unsigned dim, const double xx[], const double yy[]) 
{ 
  std::vector<TVector2> edges;

  for(unsigned iq=0; iq<dim; iq++)
    edges.push_back(TVector2(xx[iq], yy[iq]));
  
  mPolygons.push_back(edges); 
} // EtmPolygonGroup::AddPolygon()

// ---------------------------------------------------------------------------------------

void EtmPolygonGroup::AddRectangle(const TVector2 &bl, const TVector2 &tr, 
				   unsigned xsplit, unsigned ysplit)
{
  if (!xsplit) xsplit = 1; if (!ysplit) ysplit = 1;

  double x0 = bl.X(), y0 = bl.Y();
  double xstep = (tr.X() - bl.X())/xsplit, ystep = (tr.Y() - bl.Y())/ysplit;

  // FIXME: not the most economic way to do this; but it works;
  for(unsigned ix=0; ix<xsplit; ix++) {
    double xOffset = x0 + xstep*ix;

    for(unsigned iy=0; iy<ysplit; iy++) {
      double yOffset = y0 + ystep*iy;

      std::vector<TVector2> buffer;
       
      buffer.push_back(TVector2(xOffset,         yOffset)); 
      buffer.push_back(TVector2(xOffset + xstep, yOffset)); 
      buffer.push_back(TVector2(xOffset + xstep, yOffset + ystep)); 
      buffer.push_back(TVector2(xOffset,         yOffset + ystep)); 

      AddPolygon(buffer);
    } //for iy
  } //for ix
} // EtmPolygonGroup::AddRectangle()

// ---------------------------------------------------------------------------------------

void EtmPolygonGroup::AddRectangle(double x0, double y0, double xsize, double ysize, 
				unsigned xsplit, unsigned ysplit)
{
  AddRectangle(TVector2(x0 - xsize/2, y0 - ysize/2), TVector2(x0 + xsize/2, y0 + ysize/2), 
	       xsplit, ysplit);
} // EtmPolygonGroup::AddRectangle()

// ---------------------------------------------------------------------------------------

void EtmPolygonGroup::ClearPolygons( void )
{
  // NB: '&' here is essential!; well, is this call needed at all (see mPolygons.clear())?;
  for(auto &polygon: mPolygons) 
    polygon.clear();

  mPolygons.clear();
} // EtmPolygonGroup::ClearPolygons()

// ---------------------------------------------------------------------------------------

void EtmPolygonGroup::DrawPolygon(unsigned dim, const double xx[], const double yy[], bool line) const
{
  auto eic = EicToyModel::Instance();

  double xxc[dim+1], yyc[dim+1];

  for(unsigned iq=0; iq<dim; iq++) {
    auto xyc = eic->cnv(TVector2(xx[iq], yy[iq]));
    xxc[iq] = xyc.X(); yyc[iq] = xyc.Y();
  } //for iq
  xxc[dim] = xxc[0]; yyc[dim] = yyc[0];

  TPolyLine *poly = new TPolyLine(dim+1, xxc, yyc);

  poly->SetLineColor(mLineColor);
  poly->SetFillColor(mFillColor);
  poly->SetLineWidth(mLineWidth);
  poly->SetLineStyle(mLineStyle);
  poly->Draw("F");
  if (line) poly->Draw();
} // EtmPolygonGroup::DrawPolygon()

// ---------------------------------------------------------------------------------------

void EtmPolygonGroup::DrawPolygon(const std::vector<TVector2> &polygon, bool line, 
				  const TVector2 *shift, unsigned tb) const
{
  // Not the most efficient way, but TPolyLine requires separate xx[] and yy[] arrays anyway;
  unsigned dim = polygon.size();
  double xx[dim], yy[dim];

  for(unsigned iq=0; iq<dim; iq++) {
    TVector2 vtx = polygon[iq] + (shift ? *shift : TVector2());

    xx[iq] = vtx.X();//polygon[iq].X();
    yy[iq] = vtx.Y()*(tb ? -1.0 : 1.0);//polygon[iq].Y();
  } //for iq

  DrawPolygon(dim, xx, yy, line);
} // EtmPolygonGroup::DrawPolygon()

// ---------------------------------------------------------------------------------------

void EtmPolygonGroup::DrawMe(const TVector2 *shift, unsigned tb) const 
{
  for(auto polygon: mPolygons) 
    DrawPolygon(polygon, true, shift, tb);
} // EtmPolygonGroup::DrawMe()

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

ClassImp(EtmPolygonGroup)
