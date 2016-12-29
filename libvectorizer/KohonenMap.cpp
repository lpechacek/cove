#ifndef USING_PCH
#include <math.h>
#endif /* USING_PCH */

#include "KohonenMap.H"

//@{
//!\ingroup libvectorizer

/*! \class OrganizableElement
 * \brief Pure virtual class giving interface for organizable elements
 * represented by vectors. 
 */
OrganizableElement::~OrganizableElement() {}
/*! \fn virtual OrganizableElement* OrganizableElement::clone() const = 0;
    Creates a copy of this constant object. [Virtual Constructor] */
/*! \fn virtual double OrganizableElement::distance(const OrganizableElement& y) const = 0;
    Computes distance of this and another object. */
/*! \fn virtual double OrganizableElement::squares(const OrganizableElement& y) const = 0;
    Computes sum of squares of coordinate differences of this and other object.
    Used when computing Ward's criterion. */
/*! \fn virtual void OrganizableElement::add(const OrganizableElement& y) = 0;
    Adds another object to this. */
/*! \fn virtual void OrganizableElement::substract(const OrganizableElement& y) = 0;
    Substracts another object from this. */
/*! \fn virtual void OrganizableElement::multiply(const double y) = 0;
    Multiplies this object by given double. */

/*! \class KohonenAlphaGetter
 * \brief Pure virtual class giving interface for concrete class
 * encapsulating learning speed strategy. 
 */
KohonenAlphaGetter::~KohonenAlphaGetter() {}
/*! \fn virtual double KohonenAlphaGetter::getAlpha() = 0;
    Gives alpha (learning speed). */
/*! \fn virtual unsigned int KohonenAlphaGetter::getE() = 0;
    Gives number of steps for which the alpha is valid. */

/*! \class KohonenPatternGetter
 * \brief Pure virtual class giving interface for concrete class
 * encapsulating pattern selection strategy. */
KohonenPatternGetter::~KohonenPatternGetter() {}
/*! \fn virtual const OrganizableElement* KohonenPatternGetter::getPattern() = 0;
    Return one pattern. */

/*! \class BatchPatternGetter
 * \brief Pure virtual class giving interface for class
 * encapsulating pattern selection strategy of batch learning.
 *
 * This interface is used for batch learning process where we need to remember
 * class assignment for every element.  The class declares only accessors for
 * getting/setting the class assignment.  The only accessible element is the last
 * one returned by \a getPattern(). 
 */
BatchPatternGetter::~BatchPatternGetter() {}
/*! \fn virtual const int BatchPatternGetter::getLastElementClass() const = 0;
  Returns last patterns' class. */
/*! \fn virtual void BatchPatternGetter::setLastElementClass(int classNumber) = 0;
  Sets last patterns' class. */
/*! \fn virtual void BatchPatternGetter::reset() = 0;
  Restores pattern getter to its initial state.
  */
/*! \fn virtual int BatchPatternGetter::numberOfChanges() = 0;
  How many times elements changed their class since the last call to reset.
  */

/*! \class KohonenMap
  \brief Kohonen map. */

/*! \var int KohonenMap::nClasses
  Number of classes used in classification process. */
/*! \var OrganizableElement** KohonenMap::classes
  Array of pointers to classes' moments. */

//! Construct map recognizing nClasses many classes.
KohonenMap::KohonenMap(int nClasses):nClasses(nClasses)
{
    classes = new OrganizableElement*[nClasses];
}

//! Delete classes array upon destruction.
KohonenMap::~KohonenMap()
{
  for(int i=0; i<nClasses; i++)
    delete classes[i];
  delete [] classes;
}

//! Initialize moments.
void KohonenMap::setClasses(OrganizableElement **newClasses)
{
  for(int i = 0; i < nClasses; i++)
  {
    classes[i] = newClasses[i]->clone();
  }
}

//! Get number of moments.
int KohonenMap::getNumberOfClasses()
{
  return nClasses;
}

//! Get resulting moments.
OrganizableElement** KohonenMap::getClasses()
{
  OrganizableElement **retClasses = new OrganizableElement*[nClasses];
  for(int i = 0; i < nClasses; i++)
  {
    retClasses[i] = classes[i]->clone();
  }
  return retClasses;
}
	  
//! Find closest class (momentum).
int KohonenMap::findClosest(const OrganizableElement& v, double& bestDistance)
{
  int bestIndex = 0;
  double currentDistance;
  bestDistance = classes[0]->distance(v);
  for(int i = 0; i < nClasses; i++)
  {
    if((currentDistance = classes[i]->distance(v)) < bestDistance)
    {
      bestDistance = currentDistance;
      bestIndex = i;
    }
  }
  return bestIndex;
}

/*! Perform one learning step:
  -# find closest momentum
  -# modify it according to Kohonens' learning rule */
void KohonenMap::learn(const OrganizableElement& v, double alfa)
{
  double bd;
  int learnMomentum = findClosest(v, bd);
  OrganizableElement* adjustment = v.clone();
  adjustment->substract(*classes[learnMomentum]);
  adjustment->multiply(alfa);
  classes[learnMomentum]->add(*adjustment);
  delete adjustment;
}

//! Perform learning process.
void KohonenMap::performLearning(KohonenAlphaGetter& alphaGetter,
    KohonenPatternGetter& patternGetter)
{
  double alpha = alphaGetter.getAlpha();
  unsigned int E = alphaGetter.getE();

  while(alpha > 0)
  {
    for(unsigned int i = 0; i < E; i++)
    {
      const OrganizableElement *randomElement = patternGetter.getPattern();
      learn(*randomElement, alpha);
    }
    alpha = alphaGetter.getAlpha();
    E = alphaGetter.getE();
  }  
}

//! Perform batch learning.
double
KohonenMap::performBatchLearning(BatchPatternGetter& patternGetter)
{
  unsigned int* counts = new unsigned int[nClasses];
  OrganizableElement** accClasses = new OrganizableElement*[nClasses];
  double dist, quality;

  do 
  {
    quality = 0;
    patternGetter.reset();

    for(int i = 0; i < nClasses; i++)
    {
      counts[i] = 0;
      accClasses[i] = classes[i]->clone();
      accClasses[i]->multiply(0);
    }

    while(const OrganizableElement* element =  patternGetter.getPattern())
    {
      int learnMomentum = findClosest(*element, dist);
      accClasses[learnMomentum]->add(*element);
      counts[learnMomentum]++;
      patternGetter.setLastElementClass(learnMomentum);
      quality += element->squares(*classes[learnMomentum]);
    }

    for(int i = 0; i < nClasses; i++)
    {
      if(counts[i])
	accClasses[i]->multiply(1.0/counts[i]);
      else
	accClasses[i]->multiply(0);

      delete classes[i];
      classes[i] = accClasses[i];
    }
  }
  while(patternGetter.numberOfChanges());

  delete [] counts;
  delete [] accClasses;
  return quality;
}

//@}
