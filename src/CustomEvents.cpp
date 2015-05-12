#ifndef USING_PCH
#endif /* USING_PCH */

#include "CustomEvents.H"

//@{ 
//! \ingroup gui

/*! \class ThreadFinishedEvent
  \brief This is custom event signalling receiver that thread has finished its work. */

//! Default constructor, assigns this event type QEvent::User.
ThreadFinishedEvent::ThreadFinishedEvent() : QEvent(QEvent::User)
{
}	

/*! \class PercentageChangedEvent
  \brief This event notifies mainForm that it should update progress dialog precentage. */
/*! \var int PercentageChangedEvent::percentage 
 Value of percentage (0-100) sent. */
/*! \var QObject* PercentageChangedEvent::sender 
 */

/*! Constructor, assigns this event type QEvent::User+1.
  \param[in] s Sender object (e.g. form).
  \param[in] p Value of percentage to be displayed. */
PercentageChangedEvent::PercentageChangedEvent(QObject* s, int p):
  QEvent(static_cast<QEvent::Type>(QEvent::User+1)), percentage(p), sender(s)
{
}

//! The value of percentage this object holds.
int PercentageChangedEvent::getPercentage() const
{
  return percentage;
}

//! The sender of this notification.
QObject* PercentageChangedEvent::getSender() const
{
  return sender;
}

/*! \class CancelPressedEvent
 * \brief This event notifies UIProgressObserver that cancel button of the
 * progress dialog was pressed. */

//! Default constructor, assigns this event type QEvent::User+2.
CancelPressedEvent::CancelPressedEvent():
  QEvent(static_cast<QEvent::Type>(QEvent::User+2))
{
}	

//@}
