#ifndef __FIRFILTER_H__
#define __FIRFILTER_H__

#ifndef USING_PCH
#include <QImage>
#endif /* USING_PCH */

class ProgressObserver;

class FIRFilter 
{
  protected:
    int dimension;
    double** matrix;
    
  public:
    FIRFilter(unsigned radius = 0);
    FIRFilter(FIRFilter& other);
    ~FIRFilter();
    FIRFilter& operator=(const FIRFilter&);
    FIRFilter& binomic();
    FIRFilter& box();
    FIRFilter& a(double center);
    QImage apply(const QImage& source, 
	QRgb outOfBoundsColor = qRgb(128, 128, 128), 
	ProgressObserver* progressObserver = 0);
};

#endif
