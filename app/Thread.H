#ifndef __THREAD_H__
#define __THREAD_H__

#ifndef USING_PCH
#include <QThread>
#endif /* USING_PCH */

class UIProgressDialog;
class Vectorizer;

class ClassificationThread: public QThread
{
  protected:
    UIProgressDialog* p;
    Vectorizer* v;
    ClassificationThread();
  public:
    ClassificationThread(Vectorizer* v, UIProgressDialog* p);
    virtual void run();
};

#endif
