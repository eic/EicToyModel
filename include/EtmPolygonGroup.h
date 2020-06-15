
#include <vector>

#include <TVector2.h>

#ifndef _ETM_POLYGON_GROUP_
#define _ETM_POLYGON_GROUP_

#include <EtmOrphans.h>

class EtmPolygonGroup: public TObject {
 public:
 EtmPolygonGroup(int fillcolor = 0, int linecolor = 1): 
  mFillColor(fillcolor), mLineColor(linecolor), mLineWidth(2), 
    mTextColor(kBlack), mLineStyle(etm::solid) {};
  ~EtmPolygonGroup() { Clear(); };
  
  void AddPolygon(const std::vector<TVector2> &polygon) { mPolygons.push_back(polygon); };
  void AddPolygon(unsigned dim, const double xx[], const double yy[]);
  void AddRectangle(double x0, double y0, double xsize, double ysize, 
		    unsigned xsplit = 1, unsigned ysplit = 1);
  void AddRectangle(const TVector2 &bl, const TVector2 &tr, 
		    unsigned xsplit = 1, unsigned ysplit = 1);

  int GetFillColor( void )                                     const { return mFillColor; };
  int GetLineColor( void )                                     const { return mLineColor; };
  etm::LineStyle GetLineStyle( void )                          const { return mLineStyle; };

  void SetLineColor(int color)            { mLineColor = color; };
  void SetLineWidth(int width)            { mLineWidth = width; };
  void SetLineStyle(etm::LineStyle style) { mLineStyle = style; };
  void SetTextColor(int color)            { mTextColor = color; };

  const std::vector<std::vector<TVector2> > &Polygons( void )  const { return mPolygons; };

  void DrawPolygon(unsigned dim, const double xx[], const double yy[], bool line = true) const; 
  void DrawPolygon(const std::vector<TVector2> &polygon, bool line = true, 
		   const TVector2 *shift = 0, unsigned tb = 0) const; 
  void DrawMe(const TVector2 *shift = 0, unsigned tb = 0)      const;

  void ClearPolygons( void );

  unsigned PolygonCount( void )                                const { return mPolygons.size(); };

 protected:
  // Line color, fill color and line style is the same for all polygons;
  int mFillColor, mLineColor, mLineWidth, mTextColor;
  etm::LineStyle mLineStyle;
  
  // Polygon is just a vector of 2D vertices, for all practical purposes;
  std::vector<std::vector<TVector2> > mPolygons;

  ClassDef(EtmPolygonGroup, 3)
};

#endif
