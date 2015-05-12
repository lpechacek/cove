#include <math.h>
#include <stdexcept>

namespace std
{
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
  template<typename InputVector>
  class KohonenPatternGetter {
    public:
    virtual const InputVector *getPattern() = 0;
  };

  template<typename InputVector, typename KohonenAlphaGetter,
    typename KohonenPatternGetter>
  class KohonenMap {
    protected:
      int nClasses;
      InputVector **classes;

    private:
      // default constructor is not available
      KohonenMap();

    public:
      // construct map recognizing nClasses many classes
      KohonenMap(int nClasses):nClasses(nClasses) {
	classes = new InputVector*[nClasses];
      }

      // delete classes array upon destruction
      ~KohonenMap() {
	delete [] classes;
      }

      // initialize moments
      void setClasses(InputVector **newClasses) {
	for(int i=0; i<nClasses; i++) {
	  classes[i] = newClasses[i];
	}
      }

      // get number of moments
      int getNumberOfClasses() {
	return nClasses;
      }

      // get resulting moments
      InputVector **getClasses() {
	InputVector **retClasses = new InputVector*[nClasses];
	for(int i=0; i<nClasses; i++) {
	  retClasses[i] = classes[i];
	}
	return retClasses;
      }
	  
      // find closest class (momentum)
      int findClosest(const InputVector *v) {
	int bestIndex = 0;
	double bestDistance = classes[0]->distance(*v), currentDistance;
	for(int i=0; i<nClasses; i++) {
	  if((currentDistance = classes[i]->distance(*v)) < bestDistance) {
	    bestDistance = currentDistance;
	    bestIndex = i;
	  }
	}
	return bestIndex;
      }

      // perform one learning step
      // 1) find closest momentum
      // 2) modify it according to Kohonen learning rule
      void learn(const InputVector *v, double alfa) {
	int learnMomentum = findClosest(v);
	*classes[learnMomentum] = *classes[learnMomentum]+
	    (*v-(*classes[learnMomentum]))*alfa;
      }

      // perform learning process
      void performLearning(KohonenAlphaGetter &alphaGetter,
	  KohonenPatternGetter &patternGetter) {
	double alpha = alphaGetter.getAlpha();
	unsigned int E = alphaGetter.getE();

	while (alpha > 0) {
	  for(unsigned int i = 0; i < E; i++) {
	    const InputVector *randomVector = patternGetter.getPattern();
	    learn(randomVector, alpha);
	  }
	  alpha = alphaGetter.getAlpha();
	  E = alphaGetter.getE();
	}
      }
  };
}
