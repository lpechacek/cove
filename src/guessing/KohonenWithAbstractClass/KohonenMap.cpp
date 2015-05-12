#include <math.h>
#include "KohonenMap.hpp"

using namespace std;

KohonenMap::KohonenMap(int nClasses):nClasses(nClasses) {
    classes = new InputVector*[nClasses];
}

KohonenMap::~KohonenMap() {
    delete [] classes;
}

void KohonenMap::setClasses(InputVector **newClasses) {
  for(int i=0; i<nClasses; i++) {
    classes[i] = newClasses[i];
  }
}

int KohonenMap::getNumberOfClasses() {
  return nClasses;
}

InputVector **KohonenMap::getClasses() {
  InputVector **retClasses = new InputVector*[nClasses];
  for(int i=0; i<nClasses; i++) {
    retClasses[i] = classes[i];
  }
  return retClasses;
}
	  
int KohonenMap::findClosest(const InputVector *v) {
  int bestIndex = 0;
  double bestDistance = classes[0]->distance(v), currentDistance;
  for(int i=0; i<nClasses; i++) {
    if((currentDistance = classes[i]->distance(v)) < bestDistance) {
      bestDistance = currentDistance;
      bestIndex = i;
    }
  }
  return bestIndex;
}

void KohonenMap::learn(const InputVector *v, double alfa) {
  int learnMomentum = findClosest(v);
  InputVector* adjustment = v->clone();
  adjustment->substract(classes[learnMomentum]);
  adjustment->multiply(alfa);
  classes[learnMomentum]->add(adjustment);
  delete adjustment;
}

void KohonenMap::performLearning(KohonenAlphaGetter &alphaGetter,
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
