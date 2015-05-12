#ifndef __KOHONENMAP_HPP__
#define __KOHONENMAP_HPP__

#include <stdexcept>

namespace std {
  // Pure virtual class giving interface for organizable elements represented
  // by vectors.  The binary operators +,-,* and function distance must be defined.
  class InputVector {
    public:
    virtual InputVector* clone() const = 0;
    virtual double distance(const InputVector *y) = 0;
    virtual void add(const InputVector *y) = 0;
    virtual void substract(const InputVector *y) = 0;
    virtual void multiply(const double y) = 0;
  };

  // Pure virtual class giving interface for concrete class encapsulating
  // learning speed strategy.
  // double getAlpha() - gives alpha (learning speed)
  // unsigned int getE() - gives number of steps for which the alpha is valid
  class KohonenAlphaGetter {
    public:
    virtual double getAlpha() = 0;
    virtual unsigned int getE() = 0;
  };

  // Pure virtual class giving interface for concrete class encapsulating
  // pattern selection strategy.
  // const InputVector *getPattern() - return one pattern
  class KohonenPatternGetter {
    public:
    virtual const InputVector *getPattern() = 0;
  };

  class KohonenMap {
    protected:
      int nClasses;
      InputVector **classes;

    private:
      // default constructor is not available
      KohonenMap();

    public:
      // construct map recognizing nClasses many classes
      KohonenMap(int nClasses);

      // delete classes array upon destruction
      ~KohonenMap();

      // initialize moments
      void setClasses(InputVector **newClasses);

      // get number of moments
      int getNumberOfClasses();

      // get resulting moments
      InputVector **getClasses();
	  
      // find closest class (momentum)
      int findClosest(const InputVector *v);

      // perform one learning step
      // 1) find closest momentum
      // 2) modify it according to Kohonen learning rule
      void learn(const InputVector *v, double alfa);

      // perform learning process
      void performLearning(KohonenAlphaGetter &alphaGetter,
	      KohonenPatternGetter &patternGetter);
  };
}
#endif
