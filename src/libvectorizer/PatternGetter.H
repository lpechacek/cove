#ifndef __PATTERNGETTER_H__
#define __PATTERNGETTER_H__

#ifndef USING_PCH
#include <QImage>
#endif /* USING_PCH */

#include "KohonenMap.H"

class MapColor;
class ProgressObserver;

class PatternGetterDataMembers
{
  protected:
    const QImage& image;
    MapColor* retval;
    int width, height;
    PatternGetterDataMembers(const QImage& i, MapColor* mc);
    ~PatternGetterDataMembers();
};

class RandomPatternGetter: public KohonenPatternGetter,
			   protected PatternGetterDataMembers
{
  private:
    RandomPatternGetter();

  public:
    RandomPatternGetter(const QImage& im, MapColor* mc);
    virtual ~RandomPatternGetter();
    virtual const OrganizableElement* getPattern();
};

class SequentialPatternGetter: public BatchPatternGetter,
			       protected PatternGetterDataMembers
{
  protected:
    QImage classifiedImage;
    ProgressObserver* progressObserver;
    int x, y, nChanges;

  private:
    SequentialPatternGetter();

  public:
    SequentialPatternGetter(const QImage& im, MapColor* mc,
	ProgressObserver* progressObserver = 0);
    virtual ~SequentialPatternGetter();
    virtual const OrganizableElement* getPattern();
    virtual const int getLastElementClass() const;
    virtual void setLastElementClass(int classNumber);
    virtual void reset();
    virtual int numberOfChanges();
    virtual QImage* getClassifiedImage();
};

#endif
