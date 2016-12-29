#ifndef __THINNER_H__
#define __THINNER_H__

#ifndef USING_PCH
#include <QImage>
#endif /* USING_PCH */

class ProgressObserver;

class Morphology
{
  protected:
    static unsigned int masks[];
    static bool todelete[];
    static bool isDeletable[];
    static bool isInsertable[];
    static bool isPrunable[];
    const QImage* image;
    QImage* thinnedImage;
    bool runMorpholo(bool* table, bool insert,
	ProgressObserver* progressObserver = 0);
    int modifyImage(bool* table, bool insert,
       	ProgressObserver* progressObserver = 0);

  public:
    Morphology(const QImage* img);
    bool rosenfeld(ProgressObserver* progressObserver = 0);
    bool erosion(ProgressObserver* progressObserver = 0);
    bool dilation(ProgressObserver* progressObserver = 0);
    bool pruning(ProgressObserver* progressObserver = 0);
    QImage* getImage() const;
};

#endif
