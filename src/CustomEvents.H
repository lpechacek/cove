#ifndef __CUSTOMEVENTS_H__
#define __CUSTOMEVENTS_H__

#ifndef USING_PCH
#include <QEvent>
#endif /* USING_PCH */

class QObject;

class ThreadFinishedEvent: public QEvent
{
  public:
    ThreadFinishedEvent();
};

class PercentageChangedEvent: public QEvent
{
  protected:
    int percentage;
    QObject* sender;
    PercentageChangedEvent();

  public:
    PercentageChangedEvent(QObject* s, int p);
    int getPercentage() const;
    QObject* getSender() const;
};

class CancelPressedEvent: public QEvent
{
  public:
    CancelPressedEvent();
};

#endif
