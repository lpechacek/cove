#ifndef __MAPCOLOR_H__
#define __MAPCOLOR_H__

#ifndef USING_PCH
#include <QColor>
#endif /* USING_PCH */

#include "KohonenMap.H"

class MapColor: public OrganizableElement {
  private:
    typedef double (MapColor::*DistImplPtr)(double, double, double) const;
    DistImplPtr currentDistImpl;
    double distImplHamming(double y1, double y2, double y3) const;
    double distImplEuclid(double y1, double y2, double y3) const;
    inline double squaresImpl(double y1, double y2, double y3) const;
    double distImplChebyshev(double y1, double y2, double y3) const;
    double distImplMinkowski(double y1, double y2, double y3) const;

  protected:
    double x1, x2, x3, p, p1;
    MapColor(double ip = 2);
    MapColor(int i1, int i2, int i3, double ip = 2);

  public:
    virtual ~MapColor();
    virtual double distance(const OrganizableElement& y) const;
    virtual double squares(const OrganizableElement& y) const;
    virtual void add(const OrganizableElement& y);
    virtual void substract(const OrganizableElement& y);
    virtual void multiply(const double y);
    virtual QRgb getRGBTriplet() const = 0;
    virtual void setRGBTriplet(const QRgb i) = 0;
    void setP(double p);
    double getP();
};

class MapColorRGB: public MapColor {
  public:
    MapColorRGB(double ip = 2);
    MapColorRGB(QRgb i, double ip = 2);
    virtual OrganizableElement* clone() const;
    virtual QRgb getRGBTriplet() const;
    virtual void setRGBTriplet(const QRgb i);
};

class MapColorHSV: public MapColor {
  public:
    MapColorHSV(double ip = 2);
    MapColorHSV(QRgb i, double ip = 2);
    virtual OrganizableElement* clone() const;
    virtual QRgb getRGBTriplet() const;
    virtual void setRGBTriplet(const QRgb i);
};

#endif
