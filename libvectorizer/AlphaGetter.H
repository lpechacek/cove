#ifndef __ALPHAGETTER_H__
#define __ALPHAGETTER_H__

#include "KohonenMap.H"

class ProgressObserver;

class ClassicAlphaGetter: public KohonenAlphaGetter
{
  double alpha, minAlpha, q;
  unsigned int E;
  ProgressObserver* progressObserver;

  public:
  ClassicAlphaGetter();
  ClassicAlphaGetter(ProgressObserver* progressObserver);
  ClassicAlphaGetter(double alpha, double q, unsigned int e, double minAlpha);
  ClassicAlphaGetter(double alpha, double q, unsigned int e, double minAlpha,
      ProgressObserver* progressObserver);
  virtual double getAlpha();
  virtual unsigned int getE();

  virtual void setAlpha(double alpha);
  virtual void setMinAlpha(double minAlpha);
  virtual void setQ(double alpha);
  virtual void setE(unsigned int alpha);
};

#endif
