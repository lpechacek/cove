#ifndef __KOHONENMAP_H__
#define __KOHONENMAP_H__

class OrganizableElement {
  public:
    virtual ~OrganizableElement();
    virtual OrganizableElement* clone() const = 0;
    virtual double distance(const OrganizableElement& y) const = 0;
    virtual double squares(const OrganizableElement& y) const = 0;
    virtual void add(const OrganizableElement& y) = 0;
    virtual void substract(const OrganizableElement& y) = 0;
    virtual void multiply(const double y) = 0;
};

class KohonenAlphaGetter {
  public:
    virtual ~KohonenAlphaGetter();
    virtual double getAlpha() = 0;
    virtual unsigned int getE() = 0;
};

class KohonenPatternGetter {
  public:
    virtual ~KohonenPatternGetter();
    virtual const OrganizableElement* getPattern() = 0;
};

class BatchPatternGetter: public KohonenPatternGetter {
  public:
    virtual ~BatchPatternGetter();
    virtual int getLastElementClass() const = 0;
    virtual void setLastElementClass(int classNumber) = 0;
    virtual void reset() = 0;
    virtual int numberOfChanges() = 0;
};

class KohonenMap {
  protected:
    int nClasses;
    OrganizableElement** classes;

  private:
    KohonenMap();

  public:
    KohonenMap(int nClasses);
    ~KohonenMap();
    void setClasses(OrganizableElement** newClasses);
    int getNumberOfClasses();
    OrganizableElement** getClasses();
    int findClosest(const OrganizableElement& v, double& bestDistance);
    void learn(const OrganizableElement& v, double alfa);
    void performLearning(KohonenAlphaGetter& alphaGetter,
	KohonenPatternGetter& patternGetter);
    double performBatchLearning(BatchPatternGetter &patternGetter);
};
#endif
